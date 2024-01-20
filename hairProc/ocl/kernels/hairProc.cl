typedef float3 mat3[3];
typedef float4 mat4[4];

static void Mat3Load3(
    size_t idx,
    const __global float *a,
    mat3 m
)
{
    m[0] = vload3(idx * 3, a);
    m[1] = vload3(idx * 3 + 1, a);
    m[2] = vload3(idx * 3 + 2, a);
}

static void Mat4Load(
    size_t idx,
    const __global float *a,
    mat4 m
)
{
    m[0] = vload4(idx * 4, a);
    m[1] = vload4(idx * 4 + 1, a);
    m[2] = vload4(idx * 4 + 2, a);
    m[3] = vload4(idx * 4 + 3, a);
}

static void Mat3Store(
    const mat3 m,
    size_t idx,
    __global float *a
)
{
    vstore3(m[0], idx * 3, a);
    vstore3(m[1], idx * 3 + 1, a);
    vstore3(m[2], idx * 3 + 2, a);
}

static void Mat3FromCols(const float3 c0, const float3 c1, const float3 c2, mat3 m)
{
    m[0] = (float3)(c0.s0, c1.s0, c2.s0);
    m[1] = (float3)(c0.s1, c1.s1, c2.s1);
    m[2] = (float3)(c0.s2, c1.s2, c2.s2);
}

static void Mat3Transpose(
    const mat3 a,
    mat3 b
)
{
    Mat3FromCols(a[0], a[1], a[2], b);
}

static void Mat4FromCols(const float4 c0, const float4 c1, const float4 c2, const float4 c3, mat4 m)
{
    m[0] = (float4)(c0.s0, c1.s0, c2.s0, c3.s0);
    m[1] = (float4)(c0.s1, c1.s1, c2.s1, c3.s1);
    m[2] = (float4)(c0.s2, c1.s2, c2.s2, c3.s2);
    m[3] = (float4)(c0.s3, c1.s2, c2.s2, c3.s2);
}

static void Mat4Transpose(
    const mat4 a,
    mat4 b
)
{
    Mat4FromCols(a[0], a[1], a[2], a[3], b);
}

static void CalcPrimFrame(
    const float3 p0,
    const float3 p1,
    const float3 p2,
    mat3 m
)
{
    __private float3 z = normalize(p2 - p0);
    __private float3 y = normalize(cross(p1 - p0, z));
    __private float3 x = normalize(cross(y, z));

    Mat3FromCols(x, y, z, m);
}

static float3 Mat3VecMul(
    const float3 vec,
    const mat3 m
)
{
    return (float3)(dot(m[0], vec), dot(m[1], vec), dot(m[2], vec));
}

static float3 Mat4VecMul(
    const float3 vec,
    const mat4 m
)
{
    float4 tmp = (float4)(vec.xyz, 1);
    return (float3)(dot(m[0], tmp), dot(m[1], tmp), dot(m[2], tmp));
}

static void MatMul(
    const mat3 a,
    const mat3 b,
    mat3 m
)
{
    m[0] = (float3)(dot(a[0], b[0]), dot(a[1], b[0]), dot(a[2], b[0]));
    m[1] = (float3)(dot(a[0], b[1]), dot(a[1], b[1]), dot(a[2], b[1]));
    m[2] = (float3)(dot(a[0], b[2]), dot(a[1], b[2]), dot(a[2], b[2]));
}

static float3 quadBarycentric(
    const float2 uv,
    const float3 p0,
    const float3 p1,
    const float3 p2,
    const float3 p3
)
{
    float u = uv.x;
    float v = uv.y;
    return (float3) p0*(1-u)*(1-v) + p1*(1-u)*v + p2*u*v + p3*u*(1-v);
}


static float3 triBarycentric(
    const float2 uv,
    const float3 p0,
    const float3 p1,
    const float3 p2
)
{
    float u = uv.x;
    float v = uv.y;
    float w = 1.0f-u-v;
    return (float3) p0*w + p1*u + p2*v;
}


__kernel void HairProc(
    __global float* result,
    __global float* src_p,              // Source point positions. Since P is a vector3, length will be 3 * src_p_indices.
    __global int* src_primpts_lengths,  // How many points each prim consists of.
    __global int* src_primpts_indices,  // Array containing the first point index of each prim

    /* Targets */
    __global float* tgt_p,
    __global int* tgt_prmpts_lengths,
    __global int* tgt_prmpts_indices,
    __global int* tgt_prmpts_offset,

    __global float* tgt_rest_frames,
    __global float* tgt_frames,

    /* Capture attributes */
    __global float* capt_uv, // vector2
    __global int* capt_prm,
    __global int* unique_prm
)
{
    // idx will be our loop over each hair strand.
    int idx = get_global_id(0);

    int tgt_prm = unique_prm[capt_prm[idx]];

    // Start by getting the new uv and normal values of the target prim
    int pt0 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm]];
    int ptlast = pt0 + tgt_prmpts_lengths[tgt_prm];
    int npts = ptlast - pt0;

    if (npts != 3 && npts != 4) {
        return;
    }

    int pt1 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 1];
    int pt2 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 2];
    int pt3;

    float3 p0 = vload3(pt0, tgt_p);
    float3 p1 = vload3(pt1, tgt_p);
    float3 p2 = vload3(pt2, tgt_p);
    float3 p3;

    float2 uv = vload2(idx, capt_uv);

    float3 new_pos;
    if (npts == 3) {
        new_pos = triBarycentric(uv, p0, p1, p2);
    } else {
        pt3 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 3];
        p3 = vload3(pt3, tgt_p);
        new_pos = quadBarycentric(uv, p0, p1, p2, p3);
    }

    int src_start = src_primpts_indices[idx];
    int src_end = src_start + src_primpts_lengths[idx];

    float3 root = vload3(src_start, src_p);
    float3 offset = new_pos - root;

    mat3 R;
    mat3 F;
    Mat3Load3(capt_prm[idx], tgt_rest_frames, R);
    Mat3Load3(capt_prm[idx], tgt_frames, F);

    for (int pt_idx = src_start + 1; pt_idx < src_end; pt_idx++) {
        float3 p = vload3(pt_idx, src_p);

        p -= root;
        p = Mat3VecMul(p, R);
        p = Mat3VecMul(p, F);
        p += root + offset;

        vstore3(p, pt_idx, result);
    }
    root += offset;
    vstore3(root, src_start, result);
}

__kernel void CalcTargetFrames(
    __global float* tgt_p,
    const __global int* tgt_prmpts_indices,
    const __global int* tgt_prmpts_lengths,
    const __global int* tgt_prmpts_offset,

    const __global float* tgt_xform,
    const __global int* unique_prm,

    __global float* result,
    const int invert
)
{
    int idx = get_global_id(0);
    int tgt_prm = unique_prm[idx];

    // Assume at least 3 points on target prim
    int pt0 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm]];
    int pt1 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 1];
    int pt2 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 2];
    int pt3;

    float3 p0 = vload3(pt0, tgt_p);
    float3 p1 = vload3(pt1, tgt_p);
    float3 p2 = vload3(pt2, tgt_p);
    float3 p3;

    int ptlast = pt0 + tgt_prmpts_lengths[tgt_prm];
    int npts = ptlast - pt0;

    if (npts != 3 && npts != 4) {
        return;
    }

    mat4 xform;
    Mat4Load(0, tgt_xform, xform);

    mat4 xformT;
    Mat4Transpose(xform, xformT);

    p0 = Mat4VecMul(p0, xformT);
    p1 = Mat4VecMul(p1, xformT);
    p2 = Mat4VecMul(p2, xformT);
    if (npts == 4) {
        pt3 = tgt_prmpts_indices[tgt_prmpts_offset[tgt_prm] + 3];
        p3 = vload3(pt3, tgt_p);
        p3 = Mat4VecMul(p3, xformT);
        vstore3(p3, pt3, tgt_p);
    }

    mat3 frame;
    CalcPrimFrame(p0, p1, p2, frame);

    if (invert == 1) {
        mat3 frameT;
        Mat3Transpose(frame, frameT); // Invert orthogonal matrix by transposing
        Mat3Store(frameT, idx, result);
    } else {
        Mat3Store(frame, idx, result);
    }

    vstore3(p0, pt0, tgt_p);
    vstore3(p1, pt1, tgt_p);
    vstore3(p2, pt2, tgt_p);
}