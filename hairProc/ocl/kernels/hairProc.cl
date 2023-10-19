typedef float3 mat3[3];

static void MatLoad(
    size_t idx,
    const __global float *a,
    mat3 m
)
{
    m[0] = vload3(idx * 3, a);
    m[1] = vload3(idx * 3 + 1, a);
    m[2] = vload3(idx * 3 + 2, a);
}

static void MatStore(
    const mat3 m,
    size_t idx,
    __global float *a
)
{
    vstore3(m[0], idx * 3, a);
    vstore3(m[1], idx * 3 + 1, a);
    vstore3(m[2], idx * 3 + 2, a);
}

static void MatFromCols(const float3 c0, const float3 c1, const float3 c2, mat3 m)
{
    m[0] = (float3)(c0.s0, c1.s0, c2.s0);
    m[1] = (float3)(c0.s1, c1.s1, c2.s1);
    m[2] = (float3)(c0.s2, c1.s2, c2.s2);
}

static void Transpose(
    const mat3 a,
    mat3 b
)
{
    MatFromCols(a[0], a[1], a[2], b);
}

static void CalcPrimFrame(
    const float3 p0,
    const float3 p1,
    const float3 p2,
    mat3 m
)
{
    __private float3 e = normalize(p1 - p0);

    __private float3 z = normalize(p2 - p0);
    __private float3 y = normalize(cross(e, z));
    __private float3 x = normalize(cross(y, z));

    MatFromCols(x, y, z, m);
}


static float3 MatTVecMul(
    const float3 vec,
    const mat3 m
)
{
    mat3 mT;
    Transpose(m, mT);
    return (float3)(dot(mT[0], vec), dot(mT[1], vec), dot(mT[2], vec));
}

static float3 MatVecMul(
    const float3 vec,
    const mat3 m
)
{
    return (float3)(dot(m[0], vec), dot(m[1], vec), dot(m[2], vec));
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
    __global float* tgt_rest
)
{
    // idx will be our loop over each primitive index.
    int idx = get_global_id(0);

    // start is the first point index of this prim
    // end is the last point index of this prim

    // Start by getting the new uv and normal values of the target prim
    int pt0 = tgt_prmpts_indices[tgt_prmpts_offset[capt_prm[idx]]];
    int ptlast = pt0 + tgt_prmpts_lengths[capt_prm[idx]];
    int npts = ptlast - pt0;

    if (npts != 3 && npts != 4) {
        return;
    }

    int pt1 = tgt_prmpts_indices[tgt_prmpts_offset[capt_prm[idx]] + 1];
    int pt2 = tgt_prmpts_indices[tgt_prmpts_offset[capt_prm[idx]] + 2];
    int pt3;

    float3 p0 = vload3(pt0, tgt_p);
    float3 p1 = vload3(pt1, tgt_p);
    float3 p2 = vload3(pt2, tgt_p);
    float3 p3;

    float3 r0 = vload3(pt0, tgt_rest);
    float3 r1 = vload3(pt1, tgt_rest);
    float3 r2 = vload3(pt2, tgt_rest);

    float2 uv = vload2(idx, capt_uv);

    float3 new_pos;
    if (npts == 3) {
        new_pos = triBarycentric(uv, p0, p1, p2);
    } else {
        pt3 = tgt_prmpts_indices[tgt_prmpts_offset[capt_prm[idx]] + 3];
        p3 = vload3(pt3, tgt_p);
        new_pos = quadBarycentric(uv, p0, p1, p2, p3);
    }

    int src_start = src_primpts_indices[idx];
    int src_end = src_start + src_primpts_lengths[idx];

    float3 root = vload3(src_start, src_p);
    float3 offset = new_pos - root;

    mat3 restFrame;
    mat3 animFrame;
    MatLoad(idx, tgt_rest_frames, restFrame);
    MatLoad(idx, tgt_frames, animFrame);

    for (int pt_idx = src_start + 1; pt_idx < src_end; pt_idx++) {
        float3 p = vload3(pt_idx, src_p);

        p -= root;
        p = MatVecMul(p, restFrame);
        p = MatVecMul(p, animFrame);
        p += root + offset;

        vstore3(p, pt_idx, src_p);
    }
    root += offset;
    vstore3(root, src_start, src_p);
}

__kernel void CalcTargetFrames(
    __global float* tgt_p,
    __global int* tgt_prmpts_lengths,
    __global int* tgt_prmpts_indices,
    __global int* tgt_prmpts_offset,

    __global float* result,
    const int invert
)
{
    int idx = get_global_id(0);

    // Assume at least 3 points on target prim
    int pt0 = tgt_prmpts_indices[tgt_prmpts_offset[idx]];
    int pt1 = tgt_prmpts_indices[tgt_prmpts_offset[idx] + 1];
    int pt2 = tgt_prmpts_indices[tgt_prmpts_offset[idx] + 2];

    float3 p0 = vload3(pt0, tgt_p);
    float3 p1 = vload3(pt1, tgt_p);
    float3 p2 = vload3(pt2, tgt_p);

    mat3 m;
    CalcPrimFrame(p0, p1, p2, m);

    if (invert == 1) {
        mat3 mT;
        Transpose(m, mT); // Invert orthogonal matrix by transposing
        MatStore(mT, idx, result);
    } else {
        MatStore(m, idx, result);
    }
}