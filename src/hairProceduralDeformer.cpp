#include "hairProceduralDeformer.h"
#include "tokens.h"
#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE


HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
        VtArray<HdSampledDataSourceHandle> targetPtsHandle,
        VtVec3fArray up,
        VtVec2fArray uv,
        VtIntArray prim) : _targetPtsHandle(targetPtsHandle), _up(up), _uv(uv), _prim(prim) {}


VtVec3fArray HairProcHairProceduralDeformer::Deform(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {
    VtVec3fArray targetPts = _targetPtsHandle[0]->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();


    // std::cout<< "up: " << _up.size()<<std::endl;
    // std::cout<< "uv: " << _uv.size()<<std::endl;
    // std::cout<< "prim: " << _prim.size()<<std::endl;
    return pts;
}

PXR_NAMESPACE_CLOSE_SCOPE