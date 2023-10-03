#ifndef HAIRPROC_PROCEDURAL_DATA_SOURCES
#define HAIRPROC_PROCEDURAL_DATA_SOURCES

#include "pxr/pxr.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/dataSource.h"

#include "pxr/imaging/hd/primvarSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"

#include "hairProceduralDeformer.h"
#include "tokens.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

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
                HdPrimvarSchema::BuildInterpolationDataSource(_interpolation);
        }
        if (name == HdPrimvarSchemaTokens->role) {
            return
                HdPrimvarSchema::BuildRoleDataSource(_role);
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
      , _role(role) {}

    HdDataSourceBaseHandle _primvarValueSrc;
    TfToken _interpolation;
    TfToken _role;
};

class _PointsDataSource : public HdVec3fArrayDataSource {
public:
    HD_DECLARE_DATASOURCE(_PointsDataSource);

    VtValue GetValue(const Time shutterOffset) override {
        return VtValue(GetTypedValue(shutterOffset));
    }

    VtVec3fArray GetTypedValue(const Time shutterOffset) override {
        if (HdPrimvarSchema depPrimvar = _schema.GetPrimvar(HdTokens->points)) {
            HdSampledDataSourceHandle ds = depPrimvar.GetPrimvarValue();
            VtVec3fArray pts = ds->GetValue(shutterOffset).UncheckedGet<VtArray<GfVec3f>>();
            return _deformer->deform(pts);
        }
        return VtVec3fArray();
    }

    bool GetContributingSampleTimesForInterval(
            const Time startTime,
            const Time endTime,
            std::vector<Time> * const outSampleTimes) override {
        return false;
    }

private:
    _PointsDataSource(
        HdPrimvarsSchema& primvarSchema,
        HairProcHairProceduralDeformerSharedPtr deformer) 
        : _schema(primvarSchema), _deformer(deformer) {}

    HdPrimvarsSchema _schema;
    HairProcHairProceduralDeformerSharedPtr _deformer;
};


class _PrimvarOverrideDataSource : public HdContainerDataSource
{
public:
    HD_DECLARE_DATASOURCE(_PrimvarOverrideDataSource);

    TfTokenVector GetNames() override {
        if (!_inputDs) {
            return {};
        }
        return _inputDs->GetNames();
    }

    HdDataSourceBaseHandle Get(const TfToken &name) override {
        if (name == HdTokens->points) {
            return _PrimvarDataSource::New(
                _PointsDataSource::New(_schema, _deformer),
                HdPrimvarSchemaTokens->vertex,
                HdPrimvarSchemaTokens->point
            );
        }
        HdDataSourceBaseHandle result = _inputDs->Get(name);
        return result;
    }

private:
    _PrimvarOverrideDataSource(
        const HdContainerDataSourceHandle& inputDs,
        HdPrimvarsSchema& primvarSchema,
        HairProcHairProceduralDeformerSharedPtr deformer)
        : _inputDs(inputDs), _schema(primvarSchema), _deformer(deformer) {}

    HdContainerDataSourceHandle _inputDs;
    HdPrimvarsSchema _schema;
    HairProcHairProceduralDeformerSharedPtr _deformer;
};

class _HairProcDataSource : public HdContainerDataSource
{
public:
    HD_DECLARE_DATASOURCE(_HairProcDataSource);

    TfTokenVector GetNames() override
    {
        if (!_primDs) {
            return {};
        }
        return _primDs->GetNames();
    }

    HdDataSourceBaseHandle Get(const TfToken &name) override
    {
        auto result = _primDs->Get(name);
        if (name == HdPrimvarsSchemaTokens->primvars)
        {
            auto primvarSchema = HdPrimvarsSchema::GetFromParent(_primDs);
            if (auto primvarContainer = HdContainerDataSource::Cast(result)) {
                return _PrimvarOverrideDataSource::New(primvarContainer, primvarSchema, _deformer);
            }
        }
        return result;
    }

private:
    _HairProcDataSource(const SdfPath& primPath, const HdContainerDataSourceHandle &primDataSource, HairProcHairProceduralDeformerSharedPtr deformer) 
        : _primPath(primPath), _primDs(primDataSource), _deformer(deformer){}

    HdContainerDataSourceHandle _primDs;
    HairProcHairProceduralDeformerSharedPtr _deformer;
    const SdfPath& _primPath;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif