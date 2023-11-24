#include "hairProceduralDataSources.h"
#include <thread>

PXR_NAMESPACE_OPEN_SCOPE

TfTokenVector _PrimvarDataSource::GetNames() {
    return {HdPrimvarSchemaTokens->primvarValue,
            HdPrimvarSchemaTokens->interpolation,
            HdPrimvarSchemaTokens->role};
}

HdDataSourceBaseHandle _PrimvarDataSource::Get(const TfToken &name) {
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

VtValue _PointsDataSource::GetValue(const Time shutterOffset) {
    return VtValue(GetTypedValue(shutterOffset));
}

VtVec3fArray _PointsDataSource::GetTypedValue(const Time shutterOffset) {
    if (_schema.GetPrimvar(HdTokens->points)) {
        return _deformer->Deform(shutterOffset);
    }
    return VtVec3fArray();
}

bool _PointsDataSource::GetContributingSampleTimesForInterval(
        const Time startTime,
        const Time endTime,
        std::vector<Time> * const outSampleTimes) {
    return false;
}

TfTokenVector _PrimvarOverrideDataSource::GetNames() {
    if (!_inputDs) {
        return {};
    }
    return _inputDs->GetNames();
}

HdDataSourceBaseHandle _PrimvarOverrideDataSource::Get(const TfToken& name) {
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

TfTokenVector _HairProcDataSource::GetNames(){
    if (!_primDs) {
        return {};
    }
    return _primDs->GetNames();
}

HdDataSourceBaseHandle _HairProcDataSource::Get(const TfToken& name) {
    auto result = _primDs->Get(name);
    if (name == HdPrimvarsSchemaTokens->primvars) {
        auto primvarSchema = HdPrimvarsSchema::GetFromParent(_primDs);
        if (auto primvarContainer = HdContainerDataSource::Cast(result)) {
            return _PrimvarOverrideDataSource::New(primvarContainer, primvarSchema, _deformer);
        }
    }
    return result;
}

PXR_NAMESPACE_CLOSE_SCOPE