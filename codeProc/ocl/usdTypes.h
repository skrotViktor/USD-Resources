#ifndef USD_TYPES_H
#define USD_TYPES_H
// GfVec
typedef int2 GfVec2i;
typedef int3 GfVec3i;
typedef int4 GfVec4i;
#ifdef cl_khr_fp16
typedef half2 GfVec2h;
typedef half3 GfVec3h;
typedef half4 GfVec4h;
#endif
typedef float2 GfVec2f;
typedef float3 GfVec3f;
typedef float4 GfVec4f;
#ifdef cl_khr_fp64
typedef double2 GfVec2d;
typedef double3 GfVec3d;
typedef double4 GfVec4d;
#endif
// GfMatrix
typedef float2 GfMatrix2f[2];
typedef float3 GfMatrix3f[3];
typedef float4 GfMatrix4f[4];
#ifdef cl_khr_fp64
typedef double2 GfMatrix2d[2];
typedef double3 GfMatrix3d[3];
typedef double4 GfMatrix4d[4];
#endif
// GfQuat
#ifdef cl_khr_fp16
typedef half4 GfQuath;
#endif
typedef float4 GfQuatf;
#ifdef cl_khr_fp64
typedef double4 GfQuatd;
#endif

// LOAD FUNCTIONS
static GfVec2i LdGfVec2i (const int idx, const __global int *data) {
    return (GfVec2i)(data[idx * 2 + 0],data[idx * 2 + 1]);
}
static GfVec3i LdGfVec3i (const int idx, const __global int *data) {
    return (GfVec3i)(data[idx * 3 + 0],data[idx * 3 + 1],data[idx * 3 + 2]);
}
static GfVec4i LdGfVec4i (const int idx, const __global int *data) {
    return (GfVec4i)(data[idx * 4 + 0],data[idx * 4 + 1],data[idx * 4 + 2],data[idx * 4 + 3]);
}
#ifdef cl_khr_fp16
static GfVec2h LdGfVec2h (const int idx, const __global half *data) {
    return (GfVec2h)(data[idx * 2 + 0],data[idx * 2 + 1]);
}
static GfVec3h LdGfVec3h (const int idx, const __global half *data) {
    return (GfVec3h)(data[idx * 3 + 0],data[idx * 3 + 1],data[idx * 3 + 2]);
}
static GfVec4h LdGfVec4h (const int idx, const __global half *data) {
    return (GfVec4h)(data[idx * 4 + 0],data[idx * 4 + 1],data[idx * 4 + 2],data[idx * 4 + 3]);
}
#endif
static GfVec2f LdGfVec2f (const int idx, const __global float *data) {
    return (GfVec2f)(data[idx * 2 + 0],data[idx * 2 + 1]);
}
static GfVec3f LdGfVec3f (const int idx, const __global float *data) {
    return (GfVec3f)(data[idx * 3 + 0],data[idx * 3 + 1],data[idx * 3 + 2]);
}
static GfVec4f LdGfVec4f (const int idx, const __global float *data) {
    return (GfVec4f)(data[idx * 4 + 0],data[idx * 4 + 1],data[idx * 4 + 2],data[idx * 4 + 3]);
}
static void LdGfMatrix2f (const int idx, const __global float *data, GfMatrix2f out) {
    out[0] = LdGfVec2f(idx * 2 + 0, data);
	out[1] = LdGfVec2f(idx * 2 + 2, data);
}
static void LdGfMatrix3f (const int idx, const __global float *data, GfMatrix3f out) {
    out[0] = LdGfVec3f(idx * 3 + 0, data);
	out[1] = LdGfVec3f(idx * 3 + 3, data);
	out[2] = LdGfVec3f(idx * 3 + 6, data);
}
static void LdGfMatrix4f (const int idx, const __global float *data, GfMatrix4f out) {
    out[0] = LdGfVec4f(idx * 4 + 0, data);
	out[1] = LdGfVec4f(idx * 4 + 4, data);
	out[2] = LdGfVec4f(idx * 4 + 8, data);
	out[3] = LdGfVec4f(idx * 4 + 12, data);
}
#ifdef cl_khr_fp64
static GfVec2d LdGfVec2d (const int idx, const __global double *data) {
    return (GfVec2d)(data[idx * 2 + 0],data[idx * 2 + 1]);
}
static GfVec3d LdGfVec3d (const int idx, const __global double *data) {
    return (GfVec3d)(data[idx * 3 + 0],data[idx * 3 + 1],data[idx * 3 + 2]);
}
static GfVec4d LdGfVec4d (const int idx, const __global double *data) {
    return (GfVec4d)(data[idx * 4 + 0],data[idx * 4 + 1],data[idx * 4 + 2],data[idx * 4 + 3]);
}
static void LdGfMatrix2d (const int idx, const __global double *data, GfMatrix2d out) {
    out[0] = LdGfVec2d(idx * 2 + 0, data);
	out[1] = LdGfVec2d(idx * 2 + 2, data);
}
static void LdGfMatrix3d (const int idx, const __global double *data, GfMatrix3d out) {
    out[0] = LdGfVec3d(idx * 3 + 0, data);
	out[1] = LdGfVec3d(idx * 3 + 3, data);
	out[2] = LdGfVec3d(idx * 3 + 6, data);
}
static void LdGfMatrix4d (const int idx, const __global double *data, GfMatrix4d out) {
    out[0] = LdGfVec4d(idx * 4 + 0, data);
	out[1] = LdGfVec4d(idx * 4 + 4, data);
	out[2] = LdGfVec4d(idx * 4 + 8, data);
	out[3] = LdGfVec4d(idx * 4 + 12, data);
}
#endif

// STORE FUNCTIONS
static void StGfVec2i (const GfVec2i val, const int idx, __global int *buffer) {
    buffer[idx * 2 + 0] = val.x;
	buffer[idx * 2 + 1] = val.y;
}
static void StGfVec3i (const GfVec3i val, const int idx, __global int *buffer) {
    buffer[idx * 3 + 0] = val.x;
	buffer[idx * 3 + 1] = val.y;
	buffer[idx * 3 + 2] = val.z;
}
static void StGfVec4i (const GfVec4i val, const int idx, __global int *buffer) {
    buffer[idx * 4 + 0] = val.x;
	buffer[idx * 4 + 1] = val.y;
	buffer[idx * 4 + 2] = val.z;
	buffer[idx * 4 + 3] = val.w;
}
#ifdef cl_khr_fp16
static void StGfVec2h (const GfVec2h val, const int idx, __global half *buffer) {
    buffer[idx * 2 + 0] = val.x;
	buffer[idx * 2 + 1] = val.y;
}
static void StGfVec3h (const GfVec3h val, const int idx, __global half *buffer) {
    buffer[idx * 3 + 0] = val.x;
	buffer[idx * 3 + 1] = val.y;
	buffer[idx * 3 + 2] = val.z;
}
static void StGfVec4h (const GfVec4h val, const int idx, __global half *buffer) {
    buffer[idx * 4 + 0] = val.x;
	buffer[idx * 4 + 1] = val.y;
	buffer[idx * 4 + 2] = val.z;
	buffer[idx * 4 + 3] = val.w;
}
#endif
static void StGfVec2f (const GfVec2f val, const int idx, __global float *buffer) {
    buffer[idx * 2 + 0] = val.x;
	buffer[idx * 2 + 1] = val.y;
}
static void StGfVec3f (const GfVec3f val, const int idx, __global float *buffer) {
    buffer[idx * 3 + 0] = val.x;
	buffer[idx * 3 + 1] = val.y;
	buffer[idx * 3 + 2] = val.z;
}
static void StGfVec4f (const GfVec4f val, const int idx, __global float *buffer) {
    buffer[idx * 4 + 0] = val.x;
	buffer[idx * 4 + 1] = val.y;
	buffer[idx * 4 + 2] = val.z;
	buffer[idx * 4 + 3] = val.w;
}
static void StGfMatrix2f (const GfMatrix2f val, const int idx, __global float *buffer) {
    StGfVec2f(val[0], idx * 2 + 0, buffer);
	StGfVec2f(val[1], idx * 2 + 2, buffer);
}
static void StGfMatrix3f (const GfMatrix3f val, const int idx, __global float *buffer) {
    StGfVec3f(val[0], idx * 3 + 0, buffer);
	StGfVec3f(val[1], idx * 3 + 3, buffer);
	StGfVec3f(val[2], idx * 3 + 6, buffer);
}
static void StGfMatrix4f (const GfMatrix4f val, const int idx, __global float *buffer) {
    StGfVec4f(val[0], idx * 4 + 0, buffer);
	StGfVec4f(val[1], idx * 4 + 4, buffer);
	StGfVec4f(val[2], idx * 4 + 8, buffer);
	StGfVec4f(val[3], idx * 4 + 12, buffer);
}
#ifdef cl_khr_fp64
static void StGfVec2d (const GfVec2d val, const int idx, __global double *buffer) {
    buffer[idx * 2 + 0] = val.x;
	buffer[idx * 2 + 1] = val.y;
}
static void StGfVec3d (const GfVec3d val, const int idx, __global double *buffer) {
    buffer[idx * 3 + 0] = val.x;
	buffer[idx * 3 + 1] = val.y;
	buffer[idx * 3 + 2] = val.z;
}
static void StGfVec4d (const GfVec4d val, const int idx, __global double *buffer) {
    buffer[idx * 4 + 0] = val.x;
	buffer[idx * 4 + 1] = val.y;
	buffer[idx * 4 + 2] = val.z;
	buffer[idx * 4 + 3] = val.w;
}
static void StGfMatrix2d (const GfMatrix2d val, const int idx, __global double *buffer) {
    StGfVec2d(val[0], idx * 2 + 0, buffer);
	StGfVec2d(val[1], idx * 2 + 2, buffer);
}
static void StGfMatrix3d (const GfMatrix3d val, const int idx, __global double *buffer) {
    StGfVec3d(val[0], idx * 3 + 0, buffer);
	StGfVec3d(val[1], idx * 3 + 3, buffer);
	StGfVec3d(val[2], idx * 3 + 6, buffer);
}
static void StGfMatrix4d (const GfMatrix4d val, const int idx, __global double *buffer) {
    StGfVec4d(val[0], idx * 4 + 0, buffer);
	StGfVec4d(val[1], idx * 4 + 4, buffer);
	StGfVec4d(val[2], idx * 4 + 8, buffer);
	StGfVec4d(val[3], idx * 4 + 12, buffer);
}
#endif
#endif //USD_TYPES_H