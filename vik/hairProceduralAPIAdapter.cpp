#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/usdImaging/usdImaging/dataSourceAttribute.h"
#include "pxr/base/tf/stringUtils.h"

#include "hairProceduralAPIAdapter.h"
#include "hairProceduralAPI.h"
#include "hairProceduralSchema.h"
#include "tokens.h"

#include "pxr/usdImaging/usdImaging/adapterRegistry.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (hairProcedural)
    (target)
    (prim)
    (up)
    (uv)
);

TF_REGISTRY_FUNCTION(TfType) {
    typedef HairProcHairProceduralAPIAdapter Adapter;
    TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter> >();
    t.SetFactory< UsdImagingAPISchemaAdapterFactory<Adapter> >();
}

HairProcHairProceduralAPIAdapter::~HairProcHairProceduralAPIAdapter() {}

namespace {
class HairProcHairProceduralTargetDataSource : public HdPathArrayDataSource {
public:
    HD_DECLARE_DATASOURCE(HairProcHairProceduralTargetDataSource);

    HairProcHairProceduralTargetDataSource(
        const UsdRelationship& rel) : _rel(rel) {}

    VtValue GetValue(HdSampledDataSource::Time shutter) {
        return VtValue(GetTypedValue(shutter));
    }

    VtArray<SdfPath> GetTypedValue( HdSampledDataSource::Time shutter) {
        SdfPathVector paths;
        _rel.GetForwardedTargets(&paths);
        VtArray<SdfPath> vtPaths(paths.begin(), paths.end());
        return vtPaths;
    }

    bool GetContributingSampleTimesForInterval(
            HdSampledDataSource::Time startTime,
            HdSampledDataSource::Time endTime,
            std::vector<HdSampledDataSource::Time>* outSampleTimes) {
        return false;
    }

private:
    UsdRelationship _rel;
};
HD_DECLARE_DATASOURCE_HANDLES(HairProcHairProceduralTargetDataSource)


class _HairProceduralDataSource : public HdContainerDataSource {
public:
    HD_DECLARE_DATASOURCE(_HairProceduralDataSource);

    _HairProceduralDataSource(
        const UsdPrim& prim,
        const UsdImagingDataSourceStageGlobals& stageGlobals) : _api(prim), _stageGlobals(stageGlobals) {};
    
    TfTokenVector GetNames() override {
        TfTokenVector result;
        result.reserve(5);

        result.push_back(_tokens->hairProcedural);

        if (_api.GetUvAttr()) {
            result.push_back(_tokens->uv);
        }
        if (_api.GetPrimAttr()) {
            result.push_back(_tokens->prim);
        }
        if (_api.GetTargetRel()) {
            result.push_back(_tokens->target);
        }
        if (_api.GetUpAttr()) {
            result.push_back(_tokens->up);
        }
        return result;
    }

    HdDataSourceBaseHandle Get(const TfToken& name) override {
        if (name == _tokens->uv) {
            if (UsdAttribute attr = _api.GetUvAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        else if (name == _tokens->prim) {
            if (UsdAttribute attr = _api.GetPrimAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        else if (name == _tokens->up) {
            if (UsdAttribute attr = _api.GetUpAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        else if (name == _tokens->target) {
            if (UsdRelationship rel = _api.GetTargetRel()) {
                return HairProcHairProceduralTargetDataSource::New(rel);
            }
        }
        return nullptr;
    }

private:
    HairProcHairProceduralAPI _api;
    const UsdImagingDataSourceStageGlobals& _stageGlobals;
};
HD_DECLARE_DATASOURCE_HANDLES(_HairProceduralDataSource)

} // close anonymous namespace

HdContainerDataSourceHandle
HairProcHairProceduralAPIAdapter::GetImagingSubprimData(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        const UsdImagingDataSourceStageGlobals& stageGlobals) {

    std::cout<<prim.GetName()<<std::endl;
    return nullptr;
}

HdDataSourceLocatorSet
HairProcHairProceduralAPIAdapter::InvalidateImagingSubprim(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        TfTokenVector const& properties,
        const UsdImagingPropertyInvalidationType invalidationType) {

    return HdDataSourceLocatorSet();
}

PXR_NAMESPACE_CLOSE_SCOPE