

float3 quadBarycentric(const float2 uv,
                       const float3 p0,
                       const float3 p1,
                       const float3 p2,
                       const float3 p3) {
    return float3(0);
}

float3 triBarycentric(const float2 uv,
                      const float3 p0,
                      const float3 p1,
                      const float3 p2) {
    return float3(0);
}


__kernel void hairProc(__global float* src_p,              // Source point positions. Since P is a vector3, length will be 3 * src_p_indices.
                       __global int* src_primpts_lengths,  // How many points each prim consists of.
                       __global int* src_primpts_indices,  // Array containing the first point index of each prim

                       /* Targets */
                       __global float* tgt_p,
                       __global int* tgt_prmpts_lengths,
                       __global int* tgt_prmpts_indices,

                       /* Capture attributes */
                       __global float* capt_uv, // vector2
                       __global float* capt_up, // vector3
                       __global int* capt_prm   // int
)
{
    // idx will be our loop over each primitive index.
    int idx = get_global_id(0);
    // start is the first point index of this prim
    // end is the last point index of this prim

    // Start by getting the new uv and normal values of the target prim
    int tgt_start = capt_prm[idx];
    int tgt_end = tgt_start + tgt_prmpts_lengths[tgt_start];
    int npts = tgt_end - tgt_start;

    if (npts != 3 && npts != 4) {
        return;
    }

    float3 p0 = vload3(tgt_start, tgt_p);
    float3 p1 = vload3(tgt_start+1, tgt_p);
    float3 p2 = vload3(tgt_start+2, tgt_p);
    float3 p3;

    // float3 new_n = normalize(cross(p1-p0, p2-p0));
    // float3 new_pos;
    // if (npts == 3) {
    //     new_pos = triBarycentric(capt_uv[idx], p0, p1, p2);
    // } else {
    //     p3 = vload3(tgt_start+3, tgt_p);
    //     new_pos = quadBarycentric(capt_uv[idx], p0, p1, p2, p3);
    // }

    int src_start = src_primpts_indices[idx];
    int src_end = src_start + src_primpts_lengths[idx];

    float3 p = vload3(src_start, src_p);

    // Deform the vector
    p = p0;

    // Store the vector back in to src_p
    vstore3(p, src_start, src_p);

    for (int pt_idx = src_start; pt_idx < src_end; pt_idx++) {
        // Load the point position
        // float3 p = vload3(pt_idx * 3, src_p);

        // Deform the vector
        // p = p0;

        // Store the vector back in to src_p
        // vstore3(p, pt_idx * 3, src_p);
    }
}