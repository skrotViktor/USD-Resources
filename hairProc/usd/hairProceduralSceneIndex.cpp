
#include "hairProceduralDataSources.h"
#include "hairProceduralSceneIndex.h"
#include "tokens.h"

#include "pxr/pxr.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/base/trace/trace.h"
#include "pxr/base/trace/reporter.h"

#include "pxr/usdImaging/usdImaging/stageSceneIndex.h"
// #include "pxr/imaging/hd/renderIndex.h"

#include "pxr/usdImaging/usdImaging/adapterRegistry.h"
// #include "pxr/usdImaging/usdImaging/adapterManager.h"

#include <memory>
#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE


HairProcHairProceduralSceneIndexRefPtr
HairProcHairProceduralSceneIndex::New(const HdSceneIndexBaseRefPtr& inputSceneIndex) {
    return TfCreateRefPtr(new HairProcHairProceduralSceneIndex(inputSceneIndex));
}

static UsdImagingStageSceneIndexRefPtr
FindUsdImagingSceneIndex(const std::vector<HdSceneIndexBaseRefPtr>& inputScenes, int depth=0)
{
    TfRefPtr<UsdImagingStageSceneIndex> retVal;

    for (size_t i = 0; i < inputScenes.size(); i++) {
        HdSceneIndexBaseRefPtr const &sceneIdx = inputScenes[i];
        if (UsdImagingStageSceneIndexRefPtr const imagingSI = TfDynamic_cast<UsdImagingStageSceneIndexRefPtr>(sceneIdx)) {
            std::cout << std::string(depth*2, ' ') << imagingSI->GetDisplayName() << std::endl;

            retVal = imagingSI;
            break;
        }
        if (HdFilteringSceneIndexBaseRefPtr const filteringSi = TfDynamic_cast<HdFilteringSceneIndexBaseRefPtr>(sceneIdx)) {
            std::cout << std::string(depth*2, ' ') << filteringSi->GetDisplayName() << std::endl;

            retVal = FindUsdImagingSceneIndex(filteringSi->GetInputScenes(), depth+1);
            if (retVal) {
                break;
            }
        }
    }
    return retVal;
}

void dumpSceneIndexTree(HdSceneIndexBaseRefPtr sceneIndex, int depth = 0)
{
    std::cout << std::string(depth*2, ' ')
              << sceneIndex->GetDisplayName()
              << std::endl;
    auto filteringIndex = TfDynamic_cast<HdFilteringSceneIndexBaseRefPtr>(sceneIndex);
    if (filteringIndex)
        for (auto &&inputIndex : filteringIndex->GetInputScenes())
            dumpSceneIndexTree(inputIndex, depth+1);
}

HairProcHairProceduralSceneIndex::HairProcHairProceduralSceneIndex(const HdSceneIndexBaseRefPtr& inputSceneIndex) 
        : HdSingleInputFilteringSceneIndexBase(inputSceneIndex) {

    // dumpSceneIndexTree(inputSceneIndex);
}


HdSceneIndexPrim HairProcHairProceduralSceneIndex::GetPrim(const SdfPath& primPath) const {
    HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(primPath);
    if (prim.primType == HdPrimTypeTokens->basisCurves) {

        HdBasisCurvesSchema curveSchema = HdBasisCurvesSchema::GetFromParent(prim.dataSource);
        HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);
        HairProcHairProceduralSchema hairProcSchema = HairProcHairProceduralSchema::GetFromParent(prim.dataSource);

        if (curveSchema && primvarSchema && hairProcSchema) { 
            if (auto deformer = _deformerMap.find(primPath); deformer != _deformerMap.end()) {
                prim.dataSource = _HairProcDataSource::New(primPath, prim.dataSource, deformer->second);
            }
        }
    }
    return prim;
}

SdfPathVector
HairProcHairProceduralSceneIndex::GetChildPrimPaths(const SdfPath& primPath) const {
    return _GetInputSceneIndex()->GetChildPrimPaths(primPath);
}

void 
HairProcHairProceduralSceneIndex::_PrimsAdded(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::AddedPrimEntries& entries) {
    if (!_IsObserved()) {
        return;
    }

    for (const HdSceneIndexObserver::AddedPrimEntry& entry: entries) {
        if (entry.primType == HdPrimTypeTokens->basisCurves) {

            auto prim = _GetInputSceneIndex()->GetPrim(entry.primPath);
            HdBasisCurvesSchema curveSchema = HdBasisCurvesSchema::GetFromParent(prim.dataSource);
            HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);
            HairProcHairProceduralSchema hairProcSchema = HairProcHairProceduralSchema::GetFromParent(prim.dataSource);

            // std::cout<<"SCHEMA: " << hairProcSchema.IsDefined() << std::endl;
            if (curveSchema && primvarSchema && hairProcSchema) {
                std::cout<<"FOUND PRIM: " << entry.primPath << std::endl;
                _init_deformer(entry.primPath, hairProcSchema, curveSchema, primvarSchema);
            }
        }
    }

    HdSceneIndexNameRegistry& reg = HdSceneIndexNameRegistry::GetInstance();
    for(auto i: reg.GetRegisteredNames()) {
        std::cout<<"SCENE INDEX NAME REG: " << i<<std::endl;
    }

    std::cout<<"SEARCHING FOR SCENE INDEX...."<<std::endl;
    UsdImagingStageSceneIndexRefPtr usdImagingSi;
    if (auto filteringIdx = TfDynamic_cast<HdFilteringSceneIndexBaseRefPtr>(_GetInputSceneIndex())) {   
        usdImagingSi = FindUsdImagingSceneIndex(filteringIdx->GetInputScenes());
    }

    if (usdImagingSi) {
       std::cout<<"FOUND SCENE INDEX: " << usdImagingSi->GetDisplayName()<<std::endl; 
    }

    _SendPrimsAdded(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsRemoved(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::RemovedPrimEntries& entries) {
    if (!_IsObserved()) {
        return;
    }
    _SendPrimsRemoved(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsDirtied(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::DirtiedPrimEntries& entries) {

    // If any prims in entries are part of _targets, we need to also dirty their sources, ie the hairProcedural prims
    if (!_IsObserved()) {
        return;
    }

    HdSceneIndexObserver::DirtiedPrimEntries dirty = entries;

    for (const HdSceneIndexObserver::DirtiedPrimEntry& entry: entries) {
        if (auto it = _targets.find(entry.primPath); it != _targets.end()) {
            for (const SdfPath& path : it->second) {

                auto prim = _GetInputSceneIndex()->GetPrim(path);
                HdPrimvarsSchema primvarSchema = HdPrimvarsSchema::GetFromParent(prim.dataSource);
                dirty.emplace_back(path, primvarSchema.GetPointsLocator());
            }
        }
    }
    _SendPrimsDirtied(dirty);
}

void HairProcHairProceduralSceneIndex::_init_deformer(
        const SdfPath& primPath,
        HairProcHairProceduralSchema& procSchema,
        HdBasisCurvesSchema& basisCurvesSchema,
        HdPrimvarsSchema& primvarSchema){

    HdPathArrayDataSourceHandle target = procSchema.GetTarget();
    VtArray<SdfPath> targets = target->GetTypedValue(0);
    if (!targets.size()) {
        return;
    }

    VtArray<HdContainerDataSourceHandle> targetDs;
    for (auto it: targets) {
        HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(it);
        targetDs.push_back(prim.dataSource);
    }
    if (targetDs.size() == 0){
        return;
    }

    HdContainerDataSourceHandle sourceDs = _GetInputSceneIndex()->GetPrim(primPath).dataSource;
    TraceCollector::GetInstance().SetEnabled(true);

    HairProcHairProceduralDeformerSharedPtr deformer = std::make_shared<HairProcHairProceduralDeformer>(targetDs, sourceDs, primPath);

    TraceCollector::GetInstance().SetEnabled(false);
    TraceReporter::GetGlobalReporter()->ReportTimes(std::cout);

    _deformerMap[primPath] = deformer;
    for (SdfPath& path : targets) {
        _targets[path].insert(primPath);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE

