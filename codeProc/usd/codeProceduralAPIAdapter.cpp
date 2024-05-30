#include "pxr/base/tf/stringUtils.h"
#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/usdImaging/usdImaging/dataSourceAttribute.h"
#include "pxr/usdImaging/usdImaging/dataSourceRelationship.h"

#include "codeProceduralAPI.h"
#include "codeProceduralAPIAdapter.h"
#include <pxr/usd/usd/attribute.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (codeProcedural)(read)(write)(readWrite)(readNames)(writeNames)(readWriteNames)(code)(workgroupSize));

TF_REGISTRY_FUNCTION(TfType) {
    typedef CodeProcCodeProceduralAPIAdapter Adapter;
    TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter>>();
    t.SetFactory<UsdImagingAPISchemaAdapterFactory<Adapter>>();
}

CodeProcCodeProceduralAPIAdapter::~CodeProcCodeProceduralAPIAdapter() {}

// DATA SOURCE OVERRIDES

class CodeProcCodeProceduralNamesDataSource : public HdTokenArrayDataSource {
  public:
    HD_DECLARE_DATASOURCE(CodeProcCodeProceduralNamesDataSource);

    CodeProcCodeProceduralNamesDataSource(const UsdAttribute &attr)
        : _attr(attr) {}

    VtValue GetValue(HdSampledDataSource::Time shutter) {
        return VtValue(GetTypedValue(shutter));
    }

    VtArray<TfToken> GetTypedValue(HdSampledDataSource::Time shutter) {
        VtArray<TfToken> tokens;
        _attr.Get(&tokens);
        return tokens;
    }

    bool GetContributingSampleTimesForInterval(
        HdSampledDataSource::Time startTime, HdSampledDataSource::Time endTime,
        std::vector<HdSampledDataSource::Time> *outSampleTimes) {
        return false;
    }

  private:
    UsdAttribute _attr;
};

HD_DECLARE_DATASOURCE_HANDLES(CodeProcCodeProceduralNamesDataSource)

class _CodeProcCodeProceduralDataSource : public HdContainerDataSource {
  public:
    HD_DECLARE_DATASOURCE(_CodeProcCodeProceduralDataSource);

    _CodeProcCodeProceduralDataSource(
        const UsdPrim &prim,
        const UsdImagingDataSourceStageGlobals &stageGlobals)
        : _api(prim), _stageGlobals(stageGlobals){};

    TfTokenVector GetNames() override {
        TfTokenVector result;
        result.reserve(5);

        result.push_back(_tokens->codeProcedural);

        if (_api.GetReadRel()) {
            result.push_back(_tokens->read);
        }
        if (_api.GetWriteRel()) {
            result.push_back(_tokens->write);
        }
        if (_api.GetReadWriteRel()) {
            result.push_back(_tokens->readWrite);
        }
        if (_api.GetReadNamesAttr()) {
            result.push_back(_tokens->readNames);
        }
        if (_api.GetWriteNamesAttr()) {
            result.push_back(_tokens->writeNames);
        }
        if (_api.GetReadWriteNamesAttr()) {
            result.push_back(_tokens->readWriteNames);
        }
        if (_api.GetCodeAttr()) {
            result.push_back(_tokens->code);
        }
        if (_api.GetWorkgroupSizeAttr()) {
            result.push_back(_tokens->workgroupSize);
        }
        return result;
    }

    HdDataSourceBaseHandle Get(const TfToken &name) override {
        if (name == _tokens->read) {
            if (UsdRelationship rel = _api.GetReadRel()) {
                return UsdImagingDataSourceRelationship::New(rel,
                                                             _stageGlobals);
            }
        } else if (name == _tokens->write) {
            if (UsdRelationship rel = _api.GetWriteRel()) {
                return UsdImagingDataSourceRelationship::New(rel,
                                                             _stageGlobals);
            }
        } else if (name == _tokens->readWrite) {
            if (UsdRelationship rel = _api.GetReadWriteRel()) {
                return UsdImagingDataSourceRelationship::New(rel,
                                                             _stageGlobals);
            }
        } else if (name == _tokens->readNames) {
            if (UsdAttribute attr = _api.GetReadNamesAttr()) {
                return CodeProcCodeProceduralNamesDataSource::New(attr);
            }
        } else if (name == _tokens->writeNames) {
            if (UsdAttribute attr = _api.GetWriteNamesAttr()) {
                return CodeProcCodeProceduralNamesDataSource::New(attr);
            }
        } else if (name == _tokens->readWriteNames) {
            if (UsdAttribute attr = _api.GetReadWriteNamesAttr()) {
                return CodeProcCodeProceduralNamesDataSource::New(attr);
            }
        } else if (name == _tokens->code) {
            if (UsdAttribute attr = _api.GetCodeAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        } else if (name == _tokens->workgroupSize) {
            if (UsdAttribute attr = _api.GetWorkgroupSizeAttr()) {
                return UsdImagingDataSourceAttributeNew(attr, _stageGlobals);
            }
        }
        return nullptr;
    }

  private:
    CodeProcCodeProceduralAPI _api;
    const UsdImagingDataSourceStageGlobals &_stageGlobals;
};
HD_DECLARE_DATASOURCE_HANDLES(_CodeProcCodeProceduralDataSource)

// } // close anonymous namespace

HdContainerDataSourceHandle
CodeProcCodeProceduralAPIAdapter::GetImagingSubprimData(
    UsdPrim const &prim, TfToken const &subprim,
    TfToken const &appliedInstanceName,
    const UsdImagingDataSourceStageGlobals &stageGlobals) {
    return HdRetainedContainerDataSource::New(
        _tokens->codeProcedural,
        _CodeProcCodeProceduralDataSource::New(prim, stageGlobals));
}

HdDataSourceLocatorSet
CodeProcCodeProceduralAPIAdapter::InvalidateImagingSubprim(
    UsdPrim const &prim, TfToken const &subprim,
    TfToken const &appliedInstanceName, TfTokenVector const &properties,
    const UsdImagingPropertyInvalidationType invalidationType) {
    return HdDataSourceLocatorSet();
}

PXR_NAMESPACE_CLOSE_SCOPE