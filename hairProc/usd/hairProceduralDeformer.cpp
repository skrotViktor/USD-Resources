#include "hairProceduralDeformer.h"
#include "tokens.h"

#include "pxr/base/trace/trace.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/basisCurvesTopologySchema.h"
#include "pxr/imaging/hd/meshSchema.h"
#include "pxr/imaging/hd/meshTopologySchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/xformSchema.h"

#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/vt/value.h"

#include <iostream>
#include <numeric>

#include "peasyocl/Context.h"
#include "peasyocl/KernelUtils.h"

PXR_NAMESPACE_OPEN_SCOPE

template <typename T> std::vector<int> ArgSort(const VtArray<T> &v) {

    // initialize original index locations
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(),
              [&v](int i1, int i2) { return v[i1] < v[i2]; });

    return idx;
}

HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
    VtArray<HdContainerDataSourceHandle> targetContainers,
    HdContainerDataSourceHandle sourceContainer, const SdfPath &primPath)
    : _targetContainers(targetContainers), _sourceContainer(sourceContainer),
      _primPath(primPath.GetAsString()) {
    InitOCL();
}

VtVec3fArray HairProcHairProceduralDeformer::Deform(
    const HdSampledDataSource::Time &shutterOffset) {
    // auto srcPrimvarsSchema =
    // HdPrimvarsSchema::GetFromParent(_sourceContainer); VtVec3fArray srcPos =
    // srcPrimvarsSchema.GetPrimvar(HdTokens->points).GetPrimvarValue()->GetValue(0).UncheckedGet<VtArray<GfVec3f>>();
    return _DeformOCL(shutterOffset);
}

VtVec3fArray HairProcHairProceduralDeformer::_DeformOCL(
    const HdSampledDataSource::Time &shutterOffset) {
    TRACE_FUNCTION();

    auto tgtPrimvarsSchema =
        HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcCurveSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema =
        HairProcHairProceduralSchema::GetFromParent(_sourceContainer);
    auto xformSchema = HdXformSchema::GetFromParent(_targetContainers[0]);

    GfMatrix4f xform = static_cast<GfMatrix4f>(
        xformSchema.GetMatrix()->GetTypedValue(shutterOffset));

    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points)
                              .GetPrimvarValue()
                              ->GetValue(shutterOffset)
                              .UncheckedGet<VtArray<GfVec3f>>();
    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points)
                              .GetPrimvarValue()
                              ->GetValue(shutterOffset)
                              .UncheckedGet<VtArray<GfVec3f>>();

    peasyocl::Context *_oclContext = peasyocl::Context::GetInstance();
    peasyocl::KernelHandle *procKernel =
        _oclContext->GetKernelHandle(_primPath + "HairProc");

    VtMatrix3fArray tgtFrames =
        _CalcTargetFrames(shutterOffset, false, tgtPos, xform);

    procKernel->SetBufferData<float>(tgtPos.data()->data(), "tgtPos");
    procKernel->SetBufferData<float>(tgtFrames.data()->data(), "frames");

    size_t global = srcCurveSchema.GetTopology()
                        .GetCurveVertexCounts()
                        ->GetTypedValue(0)
                        .size();

    int err = _oclContext->Execute(global, _primPath + "HairProc");
    if (err != CL_SUCCESS) {
        std::cout << "Failed: Could not deform OCL" << std::endl;
        return VtVec3fArray();
    }

    size_t resultBufferSize = srcPos.size() * 3;

    VtVec3fArray result;
    result.resize(srcPos.size(), GfVec3f(0));
    procKernel->ReadBufferData(result.data()->data(), "result",
                               resultBufferSize);

    _oclContext->Finish();
    return result;
}

bool HairProcHairProceduralDeformer::InitOCL() {
    TRACE_FUNCTION();

    peasyocl::utils::ClFile file =
        peasyocl::utils::ClFile::GetClFileByName("hairProc.cl");

    peasyocl::Context *_oclContext = peasyocl::Context::GetInstance();
    _oclContext->Init();
    _oclContext->AddSource(file);
    _oclContext->Build();
    peasyocl::KernelHandle *procKernel =
        _oclContext->AddKernel("HairProc", _primPath + "HairProc");
    peasyocl::KernelHandle *tgtKernel =
        _oclContext->AddKernel("CalcTargetFrames", _primPath + "TargetFrames");

    // /* TODO: Error check each Get request */
    auto tgtPrimvarsSchema =
        HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto tgtMeshSchema = HdMeshSchema::GetFromParent(_targetContainers[0]);
    auto xformSchema = HdXformSchema::GetFromParent(_targetContainers[0]);

    auto srcCurvesSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcPrimvarsSchema = HdPrimvarsSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema =
        HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    int err;
    int t = 0;

    /* TARGET */
    VtVec3fArray tgtPos = tgtPrimvarsSchema.GetPrimvar(HdTokens->points)
                              .GetPrimvarValue()
                              ->GetValue(t)
                              .UncheckedGet<VtArray<GfVec3f>>();

    VtIntArray tgtPrimIndices =
        tgtMeshSchema.GetTopology().GetFaceVertexIndices()->GetTypedValue(t);
    VtIntArray tgtPrimLengths =
        tgtMeshSchema.GetTopology().GetFaceVertexCounts()->GetTypedValue(t);
    VtIntArray tgtPrimOffset;

    size_t size = tgtPrimLengths.size();
    tgtPrimOffset.resize(size);
    int total = 0;
    for (int i = 0; i < size; i++) {
        tgtPrimOffset[i] = total;
        total += tgtPrimLengths[i];
    }

    /* CAPTURE ATTRIBUTES */
    VtVec2fArray captUv = srcProcSchema.GetParamuv()->GetTypedValue(t);
    VtIntArray captPrim = srcProcSchema.GetPrim()->GetTypedValue(t);
    VtVec3fArray captRest = srcProcSchema.GetRest()->GetTypedValue(t);

    /* SOURCE */
    VtVec3fArray srcPos = srcPrimvarsSchema.GetPrimvar(HdTokens->points)
                              .GetPrimvarValue()
                              ->GetValue(t)
                              .UncheckedGet<VtArray<GfVec3f>>();
    VtIntArray srcPrimLengths =
        srcCurvesSchema.GetTopology().GetCurveVertexCounts()->GetTypedValue(t);
    VtIntArray srcPrimIndices;

    size = srcPrimLengths.size();
    srcPrimIndices.resize(size);
    total = 0;

    for (int i = 0; i < size; i++) {
        srcPrimIndices[i] = total;
        total += srcPrimLengths[i];
    }

    std::vector<int> args = ArgSort(captPrim);
    _uniquePrims.assign(captPrim.begin(), captPrim.end());
    _sortedCaptPrims.resize(captPrim.size());

    int offset = 0;
    int count = 0;
    for (auto i : args) {
        _sortedCaptPrims[i] = offset;
        if (captPrim[i] != captPrim[i + 1] || i == args.size() - 1) {
            _uniquePrims.erase(_uniquePrims.begin() + offset,
                               _uniquePrims.begin() + offset + count);
            offset++;
            count = 0;
        } else {
            count++;
        }
    }

    err = tgtKernel->AddArgument<float>(CL_MEM_READ_WRITE, "tgtPos",
                                        captRest.size() * 3 * sizeof(float));

    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtIndices",
                                       tgtPrimIndices.size() * sizeof(int),
                                       tgtPrimIndices.data());
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtLengths",
                                       tgtPrimLengths.size() * sizeof(int),
                                       tgtPrimLengths.data());
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtOffset",
                                       tgtPrimOffset.size() * sizeof(int),
                                       tgtPrimOffset.data());

    err = tgtKernel->AddArgument<float>(CL_MEM_READ_ONLY, "tgtXform",
                                        16 * sizeof(float));
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, "unique_prims",
                                       _uniquePrims.size() * sizeof(int),
                                       _uniquePrims.data());

    err |= tgtKernel->AddArgument<float>(
        CL_MEM_WRITE_ONLY, "result", _uniquePrims.size() * 9 * sizeof(float));
    err |= tgtKernel->AddArgument<int>(CL_MEM_READ_ONLY, "invert", sizeof(int),
                                       false);

    if (err != CL_SUCCESS) {
        std::cout << "Failed to add parameters to Target Context" << std::endl;
        return false;
    }

    GfMatrix4f xform =
        static_cast<GfMatrix4f>(xformSchema.GetMatrix()->GetTypedValue(t));
    VtMatrix3fArray targetRestFrames =
        _CalcTargetFrames(0, true, captRest, xform);

    err = procKernel->AddArgument<float>(CL_MEM_WRITE_ONLY, "result",
                                         srcPos.size() * 3 * sizeof(float));
    err = procKernel->AddArgument<float>(CL_MEM_READ_ONLY, "srcPos",
                                         srcPos.size() * 3 * sizeof(float),
                                         srcPos.data()->data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "srcLengths",
                                        srcPrimLengths.size() * sizeof(int),
                                        srcPrimLengths.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "srcIndices",
                                        srcPrimIndices.size() * sizeof(int),
                                        srcPrimIndices.data());

    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, "tgtPos",
                                          tgtPos.size() * 3 * sizeof(float));
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtLengths",
                                        tgtPrimLengths.size() * sizeof(int),
                                        tgtPrimLengths.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtIndices",
                                        tgtPrimIndices.size() * sizeof(int),
                                        tgtPrimIndices.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "tgtOffsets",
                                        tgtPrimOffset.size() * sizeof(int),
                                        tgtPrimOffset.data());

    err |= procKernel->AddArgument<float>(
        CL_MEM_READ_ONLY, "restFrames", _uniquePrims.size() * 9 * sizeof(float),
        targetRestFrames.data()->data());
    err |= procKernel->AddArgument<float>(
        CL_MEM_READ_ONLY, "frames", _uniquePrims.size() * 9 * sizeof(float));

    err |= procKernel->AddArgument<float>(CL_MEM_READ_ONLY, "captUv",
                                          captUv.size() * 2 * sizeof(float),
                                          captUv.data()->data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "captPrim",
                                        _sortedCaptPrims.size() * sizeof(int),
                                        _sortedCaptPrims.data());
    err |= procKernel->AddArgument<int>(CL_MEM_READ_ONLY, "uniquePrims",
                                        _uniquePrims.size() * sizeof(int),
                                        _uniquePrims.data());

    if (err != CL_SUCCESS) {
        std::cout << "Failed to add parameters to Deformer Context"
                  << std::endl;
        return false;
    }
    return true;
}

VtMatrix3fArray HairProcHairProceduralDeformer::_CalcTargetFrames(
    const HdSampledDataSource::Time &shutterOffset, const bool invert,
    VtVec3fArray &pts, GfMatrix4f &xform) {
    TRACE_FUNCTION();
    auto tgtPrimvarsSchema =
        HdPrimvarsSchema::GetFromParent(_targetContainers[0]);
    auto srcCurveSchema = HdBasisCurvesSchema::GetFromParent(_sourceContainer);
    auto srcProcSchema =
        HairProcHairProceduralSchema::GetFromParent(_sourceContainer);

    peasyocl::Context *_oclContext = peasyocl::Context::GetInstance();
    peasyocl::KernelHandle *tgtHandle =
        _oclContext->GetKernelHandle(_primPath + "TargetFrames");

    tgtHandle->SetBufferData<float>(xform.data(), "tgtXform");
    tgtHandle->SetBufferData<float>(pts.data()->data(), "tgtPos");

    tgtHandle->SetArgument<int>("invert", (int)invert);

    const size_t global = _uniquePrims.size();

    int err = _oclContext->Execute(global, _primPath + "TargetFrames");
    if (err != CL_SUCCESS) {
        std::cout << "Failed: Could not deform OCL" << std::endl;
        return VtMatrix3fArray();
    }

    VtMatrix3fArray result;
    result.resize(global, GfMatrix3f(0));
    tgtHandle->ReadBufferData(result.data()->data(), "result");
    tgtHandle->ReadBufferData(pts.data()->data(), "tgtPos");

    _oclContext->Finish();
    return result;
}

PXR_NAMESPACE_CLOSE_SCOPE