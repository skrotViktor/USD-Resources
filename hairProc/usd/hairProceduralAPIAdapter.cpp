#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/usdImaging/usdImaging/dataSourceAttribute.h"
#include "pxr/base/tf/stringUtils.h"

#include "hairProceduralAPIAdapter.h"
#include "hairProceduralAPI.h"
#include "hairProceduralSchema.h"
#include "tokens.h"

#include "hairProceduralDataSources.h"

#include "pxr/usdImaging/usdImaging/adapterRegistry.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (hairProcedural)
    (target)
    (prim)
    (paramuv)
    (rest)
);

TF_REGISTRY_FUNCTION(TfType) {
    typedef HairProcHairProceduralAPIAdapter Adapter;
    TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter> >();
    t.SetFactory< UsdImagingAPISchemaAdapterFactory<Adapter> >();
}

HairProcHairProceduralAPIAdapter::~HairProcHairProceduralAPIAdapter() {}

// DATA SOURCE OVERRIDES

namespace {
class HairProcHairProceduralTargetDataSource : public HdPathArrayDataSource {
public:
    HD_DECLARE_DATASOURCE(HairProcHairProceduralTargetDataSource);

    HairProcHairProceduralTargetDataSource(const UsdRelationship& rel) : _rel(rel) {}

    VtValue GetValue(HdSampledDataSource::Time shutter) {
        return VtValue(GetTypedValue(shutter));
    }

    VtArray<SdfPath> GetTypedValue(HdSampledDataSource::Time shutter) {
        SdfPathVector paths;
        _rel.GetForwardedTargets(&paths);
        return VtArray<SdfPath>(paths.begin(), paths.end());
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


class HairProcHairProceduralUpDataSource : public HdVec3fArrayDataSource {
public:
    HD_DECLARE_DATASOURCE(HairProcHairProceduralUpDataSource);

    HairProcHairProceduralUpDataSource(const UsdAttribute& attr) : _attr(attr) {}

    VtValue GetValue(HdSampledDataSource::Time shutter) override {
        return VtValue(GetTypedValue(shutter));
    }

    VtVec3fArray GetTypedValue(HdSampledDataSource::Time shutter) override {
        VtVec3fArray result;
        _attr.Get(&result);
        return VtVec3fArray(result.begin(), result.end());
    }

    bool GetContributingSampleTimesForInterval(
            const Time startTime,
            const Time endTime,
            std::vector<Time> * const outSampleTimes) override {

        return false;
    }

private:
    UsdAttribute _attr;
};
HD_DECLARE_DATASOURCE_HANDLES(HairProcHairProceduralUpDataSource)


class _HairProcHairProceduralDataSource : public HdContainerDataSource {
public:
    HD_DECLARE_DATASOURCE(_HairProcHairProceduralDataSource);

    _HairProcHairProceduralDataSource(
        const UsdPrim& prim,
        const UsdImagingDataSourceStageGlobals& stageGlobals) : _api(prim), _stageGlobals(stageGlobals) {};
    
    TfTokenVector GetNames() override {
        TfTokenVector result;
        result.reserve(5);

        result.push_back(_tokens->hairProcedural);

        if (_api.GetParamuvAttr()) {
            result.push_back(_tokens->paramuv);
        }
        if (_api.GetPrimAttr()) {
            result.push_back(_tokens->prim);
        }
        if (_api.GetTargetRel()) {
            result.push_back(_tokens->target);
        }
        if (_api.GetRestAttr()) {
            result.push_back(_tokens->rest);
        }
        return result;
    }

    HdDataSourceBaseHandle Get(const TfToken& name) override {
        if (name == _tokens->paramuv) {
            if (UsdAttribute attr = _api.GetParamuvAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        else if (name == _tokens->prim) {
            if (UsdAttribute attr = _api.GetPrimAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        else if (name == _tokens->rest) {
            if (UsdAttribute attr = _api.GetRestAttr()) {
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
HD_DECLARE_DATASOURCE_HANDLES(_HairProcHairProceduralDataSource)

} // close anonymous namespace


HdContainerDataSourceHandle
HairProcHairProceduralAPIAdapter::GetImagingSubprimData(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        const UsdImagingDataSourceStageGlobals& stageGlobals) {

    return HdRetainedContainerDataSource::New(_tokens->hairProcedural, _HairProcHairProceduralDataSource::New(prim, stageGlobals));
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