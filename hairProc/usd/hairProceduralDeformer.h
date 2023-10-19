#ifndef HAIRPROC_PROCEDURAL_DEFORMER
#define HAIRPROC_PROCEDURAL_DEFORMER

#include "pxr/pxr.h"
#include "pxr/base/tf/declarePtrs.h"
#include "pxr/base/vt/value.h"
#include "pxr/base/gf/matrix3f.h"

#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/sceneIndex.h"
#include "pxr/imaging/hd/dataSource.h"

#include "hairProceduralSchema.h"
#include "api.h"

#include "ocl/DeformerContext.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(HairProcHairProceduralDeformer);

class HairProcHairProceduralDeformer {
public:
    HairProcHairProceduralDeformer(VtArray<HdContainerDataSourceHandle> targetContainers,
                                   HdContainerDataSourceHandle sourceContainer,
                                   const SdfPath& primPath);

    VtVec3fArray Deform(const HdSampledDataSource::Time& shutterOffset);

    const bool InitOCL();
    // const bool IsOCLInitialized() const;

private:
    VtArray<HdContainerDataSourceHandle> _targetContainers;
    HdContainerDataSourceHandle _sourceContainer;
    std::string _primPath;

    // DeformerContext* _oclContext = nullptr;

    VtVec3fArray _DeformOCL(const HdSampledDataSource::Time& shutterOffset);
    VtVec3fArray _Deform(const HdSampledDataSource::Time& shutterOffset) {return VtVec3fArray(); }
    VtMatrix3fArray _CalcTargetFrames(const HdSampledDataSource::Time& shutterOffset, const bool invert, VtVec3fArray& pts);
};

using HairProcHairProceduralDeformerSharedPtr = std::shared_ptr<class HairProcHairProceduralDeformer>;


PXR_NAMESPACE_CLOSE_SCOPE

#endif