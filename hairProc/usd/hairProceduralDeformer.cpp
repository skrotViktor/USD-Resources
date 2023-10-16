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

VtVec3fArray HairProcHairProceduralDeformer::Deform(const HdSampledDataSource::Time& shutterOffset) {
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(0).UncheckedGet<VtArray<GfVec3f>>();
    if (IsOCLInitialized()) {
        return _DeformOCL(shutterOffset);
    }
    return _Deform(shutterOffset);
}

VtVec3fArray HairProcHairProceduralDeformer::_DeformOCL(const HdSampledDataSource::Time& shutterOffset) {
    auto tgtPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcCurveSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema = HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    // return srcPos;

    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();
    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();
    if (!IsOCLInitialized()) {
        return srcPos;
    }

    VtIntArray captPrim = srcProcSchema.GetPrim()->GetTypedValue(shutterOffset);

    ocl::KernelHandle* procKernel = _oclContext->GetKernelHandle("HairProc");

    std::set<int> unique(captPrim.begin(), captPrim.end());
    VtMatrix3fArray tgtFrames = _CalcTargetFrames(shutterOffset, false, tgtPos);

    procKernel->SetBufferData<float>(tgtPos.data()->data(), 3, tgtPos.size() * 3);
    procKernel->SetBufferData<float>(srcPos.data()->data(), 0, srcPos.size() * 3);
    
    procKernel->SetBufferData<float>(tgtFrames.data()->data(), 8, unique.size() * 9);

    size_t global = srcCurveSchema.GetTopology().GetCurveVertexCounts()->GetTypedValue(0).size();
    int err = _oclContext->Execute(global, "HairProc");
    if (err != CL_SUCCESS) {
        std::cout<<"Failed: Could not deform OCL" << std::endl;
        return VtVec3fArray();
    }

    size_t resultBufferSize = srcPos.size() * 3;

    VtVec3fArray result;
    result.resize(srcPos.size(), GfVec3f(0));
    procKernel->ReadBufferData(result.data()->data(), 0, resultBufferSize);

    _oclContext->Finish();
    return result;
}


const bool HairProcHairProceduralDeformer::InitOCL() {
    _oclContext->AddSource("hairProc.cl");
    _oclContext->Build();
    ocl::KernelHandle* procKernel = _oclContext->AddKernel("HairProc");
    ocl::KernelHandle* tgtKernel = _oclContext->AddKernel("CalcTargetFrames");

    if (!IsOCLInitialized()) {
        return false;
    }

    /* TODO: Error check each Get request */
    auto tgtPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto tgtMeshSchema = HdMeshSchema::GetFromParent(_targetContainers[0]);
    auto srcCurvesSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema = HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    int err;
    int t = 0;

    /* TARGET */
    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(t).UncheckedGet<VtArray<GfVec3f>>();

    VtIntArray tgtPrimIndices = tgtMeshSchema.GetTopology().GetFaceVertexIndices()->GetTypedValue(t);
    VtIntArray tgtPrimLengths = tgtMeshSchema.GetTopology().GetFaceVertexCounts()->GetTypedValue(t);
    VtIntArray tgtPrimOffset;

    size_t size = tgtPrimLengths.size();
    tgtPrimOffset.resize(size);
    int total = 0;
    for (int i = 0; i < size; i++) {
        tgtPrimOffset[i] = total;
        total += tgtPrimLengths[i];
    }

    /* SOURCE */
    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(t).UncheckedGet<VtArray<GfVec3f>>();
    
    VtIntArray srcPrimLengths = srcCurvesSchema.GetTopology().GetCurveVertexCounts()->GetTypedValue(t);
    VtIntArray srcPrimIndices;

    size = srcPrimLengths.size();
    srcPrimIndices.resize(size);
    total = 0;
    for (int i = 0; i < size; i++) {
        srcPrimIndices[i] = total;
        total += srcPrimLengths[i];
    }

    /* CAPTURE ATTRIBUTES */
    VtVec2fArray captUv = srcProcSchema.GetParamuv()->GetTypedValue(t);
    VtIntArray captPrim = srcProcSchema.GetPrim()->GetTypedValue(t);
    VtVec3fArray captRest = srcProcSchema.GetRest()->GetTypedValue(t);

    std::set<int> unique(captPrim.begin(), captPrim.end());

    err  = tgtKernel->AddArgument<float>(CL_MEM_READ_ONLY, captRest.size() * 3, true, captRest.data()->data());
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimLengths.size(), true, tgtPrimLengths.data());
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimIndices.size(), true, tgtPrimIndices.data());
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimOffset.size(), true, tgtPrimOffset.data());

    err |= tgtKernel->AddArgument<float>(CL_MEM_WRITE_ONLY, unique.size() * 9, true); // result
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, 1, false);

    if (err != CL_SUCCESS) {
        std::cout<<"Failed to add parameters to Target Context"<<std::endl;
        return false;
    }

    VtMatrix3fArray targetRestFrames = _CalcTargetFrames(0, true, captRest);

    err  = procKernel->AddArgument<float>(CL_MEM_READ_WRITE, srcPos.size() * 3, true, srcPos.data()->data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, srcPrimLengths.size(), true, srcPrimLengths.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, srcPrimIndices.size(), true, srcPrimIndices.data());

    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, tgtPos.size() * 3, true);
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimLengths.size(), true, tgtPrimLengths.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimIndices.size(), true, tgtPrimIndices.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, tgtPrimOffset.size(), true, tgtPrimOffset.data());

    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, unique.size() * 9, true, targetRestFrames.data()->data());
    err |= procKernel->AddArgument<float>(CL_MEM_READ_WRITE, unique.size() * 9, true);

    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, captUv.size() * 2, true, captUv.data()->data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, captPrim.size(), true, captPrim.data());
    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, tgtPos.size() * 3, true, captRest.data()->data());

    if (err != CL_SUCCESS) {
        std::cout<<"Failed to add parameters to Deformer Context"<<std::endl;
        return false;
    }

    return IsOCLInitialized();
}

VtMatrix3fArray HairProcHairProceduralDeformer::_CalcTargetFrames(const HdSampledDataSource::Time& shutterOffset, const bool invert, VtVec3fArray& pts) {
    auto tgtPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto srcCurveSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema = HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    VtIntArray captPrim = srcProcSchema.GetPrim()->GetTypedValue(0);

    ocl::KernelHandle* tgtHandle = _oclContext->GetKernelHandle("CalcTargetFrames");

    int inv = invert ? 1 : 0;

    tgtHandle->SetBufferData<float>(pts.data()->data(), 0, pts.size() * 3);
    tgtHandle->kernel.setArg<int>(5, inv);

    std::unordered_set<int> unique(captPrim.begin(), captPrim.end());
    const size_t global = unique.size();

    int err = _oclContext->Execute(global, "CalcTargetFrames");
    if (err != CL_SUCCESS) {
        std::cout<<"Failed: Could not deform OCL" << std::endl;
        return VtMatrix3fArray();
    }

    size_t resultBufferSize = global * 9;

    VtMatrix3fArray result;
    result.resize(global, GfMatrix3f(0));
    tgtHandle->ReadBufferData(result.data()->data(), 4, resultBufferSize);
    return result;
}

const bool HairProcHairProceduralDeformer::IsOCLInitialized() const {
    if (_oclContext == nullptr) {
        return false;
    }
    return _oclContext->initialized;
}

PXR_NAMESPACE_CLOSE_SCOPE