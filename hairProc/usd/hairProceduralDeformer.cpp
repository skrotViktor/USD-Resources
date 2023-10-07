#include "hairProceduralDeformer.h"
#include "tokens.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
        VtArray<HdSampledDataSourceHandle> targetPtsHandle,
        VtVec3fArray up,
        VtVec2fArray uv,
        VtIntArray prim) : _targetPtsHandle(targetPtsHandle), _up(up), _uv(uv), _prim(prim) {

    _InitOCL();
}


VtVec3fArray HairProcHairProceduralDeformer::Deform(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {
    if (_IsOCLInitialized()) {
        return _DeformOCL(pts, shutterOffset);
    }
    return _Deform(pts, shutterOffset);
}

VtVec3fArray HairProcHairProceduralDeformer::_DeformOCL(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {
    VtVec3fArray targetPts = _targetPtsHandle[0]->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();

    return pts;
}


const bool HairProcHairProceduralDeformer::_InitOCL() const {
    
    return false;
}

const bool HairProcHairProceduralDeformer::_IsOCLInitialized() const {
    return _oclDevice != nullptr;
}

PXR_NAMESPACE_CLOSE_SCOPE