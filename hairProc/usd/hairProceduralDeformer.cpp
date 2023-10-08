#include "hairProceduralDeformer.h"
#include "tokens.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
        VtArray<HdSampledDataSourceHandle> targetPtsHandle,
        VtVec3fArray up,
        VtVec2fArray uv,
        VtIntArray prim) : _targetPtsHandle(targetPtsHandle), _up(up), _uv(uv), _prim(prim) {

    InitOCL();
}

VtVec3fArray HairProcHairProceduralDeformer::Deform(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {
    if (IsOCLInitialized()) {
        return _DeformOCL(pts, shutterOffset);
    }
    return _Deform(pts, shutterOffset);
}

VtVec3fArray HairProcHairProceduralDeformer::_DeformOCL(const VtVec3fArray& pts, const HdSampledDataSource::Time& shutterOffset) const {
    // VtVec3fArray targetPts = _targetPtsHandle[0]->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();

    return pts;
}


const bool HairProcHairProceduralDeformer::InitOCL() {
    const int size = 1024;
    _oclContext = std::make_shared<ocl::DeformerContext>("hairProc");

    float data[size];
    for (int i = 0; i < size; i++) {
        data[i] = i; 
    }
    unsigned int count = size;
    int err;

    err = _oclContext->AddParameter<float>(CL_MEM_READ_ONLY, size, data);
    err |= _oclContext->AddParameter<float>(CL_MEM_WRITE_ONLY, size);
    err |= _oclContext->AddParameter<unsigned int>(CL_MEM_READ_ONLY, 1, &count);
    if (err != CL_SUCCESS) {
        std::cout<<"Failed to add parameters"<<std::endl;
        return false;
    }

    std::array<float, size> results = _oclContext->Execute<float, size>();
    unsigned int correct = 0;
    for (int i = 0; i < size; i++) {
        if (results[i] == data[i] * data[i]) { 
            correct++; 
        }
    }

    printf("Computed '%d/%d' correct values!\n", correct, count);

    return IsOCLInitialized();
}

const bool HairProcHairProceduralDeformer::IsOCLInitialized() const {
    return _oclContext->initialized;
}

PXR_NAMESPACE_CLOSE_SCOPE