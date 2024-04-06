// GfVec
typedef int GfVec2i[2];
typedef int GfVec3i[3];
typedef int GfVec4i[4];
#ifndef cl_khr_fp16
typedef half GfVec2h[2];
typedef half GfVec3h[3];
typedef half GfVec4h[4];
#endif
typedef float GfVec2f[2];
typedef float GfVec3f[3];
typedef float GfVec4f[4];
#ifndef cl_khr_fp64
typedef double GfVec2d[2];
typedef double GfVec3d[3];
typedef double GfVec4d[4];
#endif
// GfMatrix
typedef float2 GfMatrix2f[2];
typedef double2 GfMatrix2d[2];
typedef float3 GfMatrix3f[3];
typedef double3 GfMatrix3d[3];
typedef float4 GfMatrix4f[4];
typedef double4 GfMatrix4d[4];
// GfQuat
#ifndef cl_khr_fp16
typedef half GfQuath[4];
#endif
typedef float GfQuatf[4];
#ifndef cl_khr_fp64
typedef double GfQuatd[4];
#endif
// TfToken
typedef size_t TfToken
