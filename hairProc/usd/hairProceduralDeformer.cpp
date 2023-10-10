#include "hairProceduralDeformer.h"
#include "tokens.h"

#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/meshSchema.h"
#include "pxr/imaging/hd/meshTopologySchema.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/basisCurvesTopologySchema.h"

#include "pxr/base/vt/value.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
        VtArray<HdContainerDataSourceHandle> targetContainers,
        HdContainerDataSourceHandle sourceContainer) : _targetContainers(targetContainers), _sourceContainer(sourceContainer) {
    InitOCL();
}

VtVec3fArray HairProcHairProceduralDeformer::Deform(const HdSampledDataSource::Time& shutterOffset) const {
    if (IsOCLInitialized()) {
        return _DeformOCL(shutterOffset);
    }
    return _Deform(shutterOffset);
}

VtVec3fArray HairProcHairProceduralDeformer::_DeformOCL(const HdSampledDataSource::Time& shutterOffset) const {
    auto tgtPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcCurveSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);

    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();
     _oclContext->SetBufferData<float>(tgtPos.data()->data(), 3, tgtPos.size() * 3);


    size_t global = srcCurveSchema.GetTopology().GetCurveVertexCounts()->GetTypedValue(0).size();
    int err = _oclContext->Execute(global);
    if (err != CL_SUCCESS) {
        std::cout<<"Failed: Could not deform OCL" << std::endl;
        return VtVec3fArray();
    }

    const VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();
    size_t resultBufferSize = srcPos.size() * 3;

    VtVec3fArray result;
    result.assign(srcPos.size(), GfVec3f(0));
    _oclContext->ReadBufferData(result.data()->data(), 0, resultBufferSize);

    std::cout<<result[0]<<std::endl;
    // srcPos.assign(result.begin(), result.end());
    return result;
}

// __global float* src_p,              // Source point positions. Since P is a vector3, length will be 3 * src_p_indices.
// __global int* src_primpts_lengths,  // How many points each prim consists of.
// __global int* src_primpts_indices,  // Array containing the first point index of each prim


// /* Targets */
// __global float* tgt_p,
// __global int* tgt_prmpts_lengths,
// __global int* tgt_prmpts_indices,

// /* Capture attributes */
// __global float* capt_uv, // vector2
// __global float* capt_up, // vector3
// __global int* capt_prm   // int


const bool HairProcHairProceduralDeformer::InitOCL() {
    /* TODO: Error check each Get request */
    auto tgtPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto tgtMeshSchema = HdMeshSchema::GetFromParent(_targetContainers[0]);
    auto srcCurvesSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema = HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(0).UncheckedGet<VtArray<GfVec3f>>();
    VtIntArray tgtPrimIndices = tgtMeshSchema.GetTopology().GetFaceVertexIndices()->GetTypedValue(0);
    VtIntArray tgtPrimLengths = tgtMeshSchema.GetTopology().GetFaceVertexCounts()->GetTypedValue(0);
    
    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(0).UncheckedGet<VtArray<GfVec3f>>();
    VtIntArray srcPrimLengths = srcCurvesSchema.GetTopology().GetCurveVertexCounts()->GetTypedValue(0);
    VtIntArray srcPrimIndices;

    size_t size = srcPrimLengths.size();
    srcPrimIndices.resize(size);
    int total = 0;
    for (int i = 0; i < size; i++) {
        srcPrimIndices[i] = srcPrimLengths[i] + total;
        total = srcPrimIndices[i];
    }

    VtVec2fArray captUv = srcProcSchema.GetUv()->GetTypedValue(0);
    VtVec3fArray captUp = srcProcSchema.GetUp()->GetTypedValue(0);
    VtIntArray captPrim = srcProcSchema.GetPrim()->GetTypedValue(0);

    int err;

    _oclContext = std::make_shared<ocl::DeformerContext>("hairProc");
    if (!IsOCLInitialized()) {
        return false;
    }

    err  = _oclContext->AddArgument<float>(CL_MEM_READ_WRITE, srcPos.size() * 3, true, srcPos.data()->data());
    err |= _oclContext->AddArgument<int>(CL_MEM_READ_ONLY, srcPrimLengths.size(), true, srcPrimLengths.data());
    err |= _oclContext->AddArgument<int>(CL_MEM_READ_ONLY, srcPrimIndices.size(), true, srcPrimIndices.data());

    /* TARGETS */
    err |= _oclContext->AddArgument<float>(CL_MEM_READ_ONLY, tgtPos.size() * 3, true, tgtPos.data()->data());
    err |= _oclContext->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimIndices.size(), true, tgtPrimIndices.data());    
    err |= _oclContext->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimLengths.size(), true, tgtPrimLengths.data());

    /* CAPTURE ATTRIBUTES */
    err |= _oclContext->AddArgument<float>(CL_MEM_READ_ONLY, captUv.size() * 2, true, captUv.data()->data());
    err |= _oclContext->AddArgument<float>(CL_MEM_READ_ONLY, captUp.size() * 3, true, captUp.data()->data());
    err |= _oclContext->AddArgument<int>(CL_MEM_READ_ONLY, captPrim.size(), true, captPrim.data());

    if (err != CL_SUCCESS) {
        std::cout<<"Failed to add parameters"<<std::endl;
        return false;
    }
    return IsOCLInitialized();
}

const bool HairProcHairProceduralDeformer::IsOCLInitialized() const {
    if (_oclContext == nullptr) {
        return false;
    }
    return _oclContext->initialized;
}

PXR_NAMESPACE_CLOSE_SCOPE