
#include "codeProceduralSceneIndex.h"
#include "codeProceduralDataSources.h"
#include "codeProceduralSchema.h"

#include "pxr/base/tf/debug.h"
#include "pxr/imaging/hd/types.h"

#include "pxr/usd/sdf/path.h"

PXR_NAMESPACE_OPEN_SCOPE

CodeProcCodeProceduralSceneIndexRefPtr CodeProcCodeProceduralSceneIndex::New(
    const HdSceneIndexBaseRefPtr &inputSceneIndex) {
    return TfCreateRefPtr(
        new CodeProcCodeProceduralSceneIndex(inputSceneIndex));
}

CodeProcCodeProceduralSceneIndex::CodeProcCodeProceduralSceneIndex(
    const HdSceneIndexBaseRefPtr &inputSceneIndex)
    : HdSingleInputFilteringSceneIndexBase(inputSceneIndex) {}


VtValue CodeProcCodeProceduralSceneIndex::_GetPrimvarValue(
    const SdfPath &primvarPath) const {

    HdSceneIndexPrim prim =
        _GetInputSceneIndex()->GetPrim(primvarPath.GetAbsoluteRootOrPrimPath());

    HdPrimvarsSchema primvars =
        HdPrimvarsSchema::GetFromParent(prim.dataSource);

    if (!primvars)
        return {};

    HdPrimvarSchema primvar = primvars.GetPrimvar(primvarPath.GetNameToken());
    if (!primvar)
        return {};

    // GfVec3f t{22,33,44};
    // float t = 22;
    // return VtValue(t);
    return primvar.GetPrimvarValue()->GetValue(0);
}


Deformer::Argument CodeProcCodeProceduralSceneIndex::_GetArgFromPath(
    const SdfPath &primvarPath, const Deformer::ArgumentFlags &flags,
    const TfToken &name) const {

    VtValue val = _GetPrimvarValue(primvarPath);
    TfType t = val.GetType();

    HdTupleType hdType = HdGetValueTupleType(val);
    size_t arraySize = hdType.count;
    size_t vectorSize = HdGetComponentCount(hdType.type);
    size_t dataSize = HdDataSizeOfType(hdType.type);

    // std::cout << t << " : " << arraySize << " : " << vectorSize << " : " << dataSize<<std::endl;

    return {primvarPath, flags, t, name, arraySize, vectorSize, dataSize};
}


HdSceneIndexPrim
CodeProcCodeProceduralSceneIndex::GetPrim(const SdfPath &primPath) const {
    HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(primPath);

    HdPrimvarsSchema primvarSchema =
        HdPrimvarsSchema::GetFromParent(prim.dataSource);
    CodeProcCodeProceduralSchema codeProcSchema =
        CodeProcCodeProceduralSchema::GetFromParent(prim.dataSource);

    if (!primvarSchema)
        return prim;

    // This prim is a CodeProc schema prim, so apply the CodeProc data source
    if (codeProcSchema) {
        if (auto deformer = _deformerMap.find(primPath);
            deformer != _deformerMap.end()) {
            prim.dataSource = CodeProcDataSourceContainer::New(
                primPath, prim.dataSource, deformer->second);
        }
    }

    // Find if this prim is is an argument to the another CodeProc schema prim,
    // if so, this prim should also apply the CodeProc data source
    for (const auto deformerPair : _deformerMap) {
        if (deformerPair.first == primPath)
            continue;

        Deformer::ArgumentSet args =
            deformerPair.second->GetFilteredArguments(primPath);

        if (args.empty())
            continue;

        prim.dataSource = CodeProcDataSourceContainer::New(
            primPath, prim.dataSource, deformerPair.second);
    }

    return prim;
}

SdfPathVector CodeProcCodeProceduralSceneIndex::GetChildPrimPaths(
    const SdfPath &primPath) const {
    return _GetInputSceneIndex()->GetChildPrimPaths(primPath);
}

void CodeProcCodeProceduralSceneIndex::_PrimsAdded(
    const HdSceneIndexBase &sender,
    const HdSceneIndexObserver::AddedPrimEntries &entries) {
    if (!_IsObserved()) {
        return;
    }

    _SendPrimsAdded(entries);
}

void CodeProcCodeProceduralSceneIndex::_PrimsRemoved(
    const HdSceneIndexBase &sender,
    const HdSceneIndexObserver::RemovedPrimEntries &entries) {
    if (!_IsObserved()) {
        return;
    }
    _SendPrimsRemoved(entries);
}

void CodeProcCodeProceduralSceneIndex::_PrimsDirtied(
    const HdSceneIndexBase &sender,
    const HdSceneIndexObserver::DirtiedPrimEntries &entries) {

    // If any prims in entries are part of _targets, we need to also dirty their
    // sources, ie the codeProcedural prims
    if (!_IsObserved()) {
        return;
    }

    HdSceneIndexObserver::DirtiedPrimEntries dirty = entries;
    for (auto i : dirty) {
        HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(i.primPath);

        HdPrimvarsSchema primvars =
            HdPrimvarsSchema::GetFromParent(prim.dataSource);

        if (!primvars)
            continue;

        CodeProcCodeProceduralSchema codeProc =
            CodeProcCodeProceduralSchema::GetFromParent(prim.dataSource);

        if (primvars && codeProc) {
            if (_deformerMap.find(i.primPath) == _deformerMap.end()) {
                _init_deformer(i.primPath, codeProc, primvars);
            }
        }

        // Find if this prim is the argument to a deformer, if that's the case,
        // we should mark the deformer prim dirty also
        for (const auto deformerPair : _deformerMap) {
            if (deformerPair.first == i.primPath)
                continue;

            Deformer::ArgumentSet args =
                deformerPair.second->GetFilteredArguments(i.primPath);
            for (const auto j : args) {
                dirty.emplace_back(deformerPair.first,
                                   primvars.GetDefaultLocator().Append(
                                       j.primvarPath.GetNameToken()));
            }

            prim.dataSource = CodeProcDataSourceContainer::New(
                i.primPath, prim.dataSource, deformerPair.second);
        }
    }

    _SendPrimsDirtied(dirty);
}

void CodeProcCodeProceduralSceneIndex::_init_deformer(
    const SdfPath &primPath, CodeProcCodeProceduralSchema &procSchema,
    HdPrimvarsSchema &primvarSchema) {

    HdStringDataSourceHandle codeDs = procSchema.GetCode();
    HdIntDataSourceHandle workgroupSizeDs = procSchema.GetWorkgroupSize();

    HdPathArrayDataSourceHandle readDs = procSchema.GetRead();
    HdPathArrayDataSourceHandle writeDs = procSchema.GetWrite();
    HdPathArrayDataSourceHandle readWriteDs = procSchema.GetReadWrite();

    HdTokenArrayDataSourceHandle readNamesDs = procSchema.GetReadNames();
    HdTokenArrayDataSourceHandle writeNamesDs = procSchema.GetWriteNames();
    HdTokenArrayDataSourceHandle readWriteNamesDs =
        procSchema.GetReadWriteNames();

    VtArray<SdfPath> read = readDs->GetTypedValue(0);
    VtArray<SdfPath> write = writeDs->GetTypedValue(0);
    VtArray<SdfPath> readWrite = readWriteDs->GetTypedValue(0);

    VtArray<TfToken> readNames = readNamesDs->GetTypedValue(0);
    VtArray<TfToken> writeNames = writeNamesDs->GetTypedValue(0);
    VtArray<TfToken> readWriteNames = readWriteNamesDs->GetTypedValue(0);

    // std::string code = codeDs->GetTypedValue(0);
    int workgroupSize = workgroupSizeDs->GetTypedValue(0);

    // Initiate deformer
    _deformerMap.clear();

    Deformer::CodeProcCodeProceduralDeformerSharedPtr deformer =
        std::make_shared<Deformer::CodeProcCodeProceduralDeformer>();

    const char *code = " \
#pragma OPENCL EXTENSION cl_khr_fp16 : enable \n\
#pragma OPENCL EXTENSION cl_khr_fp64 : enable \n\
#include \"usdTypes.h\" \n\
__kernel void myKernel ( \n\
                      const int globalSize,\n\
                      __global float* pts\n\
)\n\
{\n\
    // GfVec3f v = LdGfVec3f(0, pts);\n\
    // StGfVec3f(v, 0, pts);\n\
    // printf(\"%f\", \"pts[0]\");\n\
    int idx = get_global_id(0);\n\
    if (idx >= globalSize)\n\
        return;\n\
}";
    deformer->SetKernelCode(code);

    // workgroup size should always be the first argument
    deformer->SetWorkgroupSize(workgroupSize);
    int idx = 0;
    for (SdfPath &path : read) {
        Deformer::Argument arg = _GetArgFromPath(
            path, Deformer::ArgumentFlags::readable, readNames[idx++]);
        deformer->AddArgument(arg, _GetPrimvarValue(path));
    }

    idx = 0;
    for (SdfPath &path : readWrite) {
        Deformer::Argument arg =
            _GetArgFromPath(path,
                            Deformer::ArgumentFlags::readable |
                                Deformer::ArgumentFlags::writable,
                            readWriteNames[idx++]);
        deformer->AddArgument(arg, _GetPrimvarValue(path));
    }

    idx = 0;
    for (SdfPath &path : write) {
        Deformer::Argument arg = _GetArgFromPath(
            path, Deformer::ArgumentFlags::writable, writeNames[idx++]);
        deformer->AddArgument(arg, _GetPrimvarValue(path));
    }

    _deformerMap[primPath] = deformer;
}

PXR_NAMESPACE_CLOSE_SCOPE
