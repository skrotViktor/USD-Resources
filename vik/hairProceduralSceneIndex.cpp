#include "./hairProceduralSceneIndex.h"
#include "./tokens.h"
#include "./hairProceduralSchema.h"

#include "pxr/pxr.h"
#include "pxr/imaging/hd/filteringSceneIndex.h"
#include "pxr/usdImaging/usdImaging/stageSceneIndex.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/tokens.h"

#include "pxr/imaging/hd/tokens.h"


#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

HairProcHairProceduralSceneIndexRefPtr
HairProcHairProceduralSceneIndex::New(const HdSceneIndexBaseRefPtr& inputSceneIndex) {
    return TfCreateRefPtr(new HairProcHairProceduralSceneIndex(inputSceneIndex));
}

HairProcHairProceduralSceneIndex::HairProcHairProceduralSceneIndex(const HdSceneIndexBaseRefPtr& inputSceneIndex) 
        : HdSingleInputFilteringSceneIndexBase(inputSceneIndex) {}


class _PrimvarDataSource final : public HdContainerDataSource
{
public:
    HD_DECLARE_DATASOURCE(_PrimvarDataSource);

    TfTokenVector GetNames() override {
        return {HdPrimvarSchemaTokens->primvarValue,
                HdPrimvarSchemaTokens->interpolation,
                HdPrimvarSchemaTokens->role};
    }

    HdDataSourceBaseHandle Get(const TfToken &name) override {
        if (name == HdPrimvarSchemaTokens->primvarValue) {
            return _primvarValueSrc;
        }
        if (name == HdPrimvarSchemaTokens->interpolation) {
            return
                HdPrimvarSchema::BuildInterpolationDataSource(
                    _interpolation);
        }
        if (name == HdPrimvarSchemaTokens->role) {
            return
                HdPrimvarSchema::BuildRoleDataSource(
                    _role);
        }

        return nullptr;
    }

private:
    _PrimvarDataSource(
        const HdDataSourceBaseHandle &primvarValueSrc,
        const TfToken &interpolation,
        const TfToken &role)
      : _primvarValueSrc(primvarValueSrc)
      , _interpolation(interpolation)
      , _role(role){}

    HdDataSourceBaseHandle _primvarValueSrc;
    TfToken _interpolation;
    TfToken _role;
};

HdSceneIndexPrim HairProcHairProceduralSceneIndex::GetPrim(const SdfPath& primPath) const {
    HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(primPath);
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
            if (curveSchema && primvarSchema && hairProcSchema) {
                std::cout<< entry.primPath << std::endl;
            }
        }
    }

    _SendPrimsAdded(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsRemoved(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::RemovedPrimEntries& entries) {
    _SendPrimsRemoved(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsDirtied(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::DirtiedPrimEntries& entries) {
    if (!_IsObserved()) {
        return;
    }

    _SendPrimsDirtied(entries);
}

PXR_NAMESPACE_CLOSE_SCOPE

