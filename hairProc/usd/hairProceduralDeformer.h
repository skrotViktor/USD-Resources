#ifndef HAIRPROC_PROCEDURAL_DEFORMER
#define HAIRPROC_PROCEDURAL_DEFORMER

#include "pxr/pxr.h"
#include "pxr/base/tf/declarePtrs.h"
#include "pxr/base/vt/value.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/sceneIndex.h"

#include "hairProceduralSchema.h"
#include "api.h"

#include "ocl/DeformerContext.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(HairProcHairProceduralDeformer);

class HairProcHairProceduralDeformer {
public:
    HairProcHairProceduralDeformer(VtArray<HdSampledDataSourceHandle> targetPtsHandle, VtVec3fArray up, VtVec2fArray uv, VtIntArray prim);
    // ~HairProcHairProceduralDeformer() {delete _oclContext; };

    VtVec3fArray Deform(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const;

    const bool InitOCL();
    const bool IsOCLInitialized() const;

private:
    VtArray<HdSampledDataSourceHandle> _targetPtsHandle;
    VtVec3fArray _up;
    VtVec2fArray _uv;
    VtIntArray _prim;

    std::shared_ptr<ocl::DeformerContext> _oclContext;

    VtVec3fArray _DeformOCL(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const;
    VtVec3fArray _Deform(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {return pts;}

    // VtMatrix3fArray _ComputeFrames() {}
};

using HairProcHairProceduralDeformerSharedPtr = std::shared_ptr<class HairProcHairProceduralDeformer>;


PXR_NAMESPACE_CLOSE_SCOPE

#endif