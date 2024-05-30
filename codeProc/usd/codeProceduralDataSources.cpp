#include "codeProceduralDataSources.h"
#include "codeProceduralDeformerArguments.h"
#include "pxr/imaging/hd/containerDataSourceEditor.h"
#include <pxr/imaging/hd/types.h>
// #include "pxr/base/vt/typeHeaders.h" // IWYU pragma: keep.

PXR_NAMESPACE_OPEN_SCOPE

/* --------------------- CodeProcPrimvarValueDataSource --------------------- */

template <class T>
VtValue
CodeProcPrimvarValueDataSource<T>::GetValue(HdSampledDataSource::Time t) {
    return VtValue(GetTypedValue(t));
}

template <class T>
T CodeProcPrimvarValueDataSource<T>::GetTypedValue(
    HdSampledDataSource::Time t) {
    peasyocl::Context *ctx = peasyocl::Context::GetInstance();

    if (ctx->IsValid()) {
        T resultValue = _deformer->GetSolvedValue<T>(_argument);
        return resultValue;
    }

    return {};
}

template <class T>
bool CodeProcPrimvarValueDataSource<T>::GetContributingSampleTimesForInterval(
    HdSampledDataSource::Time startTime, HdSampledDataSource::Time endTime,
    std::vector<HdSampledDataSource::Time> *outSampleTimes) {
    return true;
}

/* -------------------- CodeProcPrimvarOverrideDataSource ------------------- */

TfTokenVector CodeProcPrimvarOverrideDataSource::GetNames() {
    return {HdPrimvarSchemaTokens->primvarValue,
            HdPrimvarSchemaTokens->interpolation, HdPrimvarSchemaTokens->role};
}

HdDataSourceBaseHandle
CodeProcPrimvarOverrideDataSource::Get(const TfToken &name) {

    if (name == HdPrimvarSchemaTokens->primvarValue) {
        return _valueDs;
    }
    if (name == HdPrimvarSchemaTokens->interpolation) {
        return HdPrimvarSchema::BuildInterpolationDataSource(_interpolation);
    }
    if (name == HdPrimvarSchemaTokens->role) {
        return HdPrimvarSchema::BuildRoleDataSource(_role);
    }
    return nullptr;
}
/* -------------------- CodeProcPrimvarsOverrideDataSource -------------------
 */

TfTokenVector CodeProcPrimvarsOverrideDataSource::GetNames() {
    if (!_inputDs) {
        return {};
    }
    return _inputDs->GetNames();
}

HdDataSourceBaseHandle
CodeProcPrimvarsOverrideDataSource::Get(const TfToken &name) {

    Deformer::ArgumentSet args = _deformer->GetFilteredArguments(_primPath);

    if (args.empty()) {
        return _inputDs->Get(name);
    }

    Deformer::ArgumentSet::iterator arg = args.begin();

    if (name != arg->primvarPath.GetNameToken()) {
        return _inputDs->Get(name);
    }

    static const _FactoryMap _factoryMap = _CreateFactoryMap();

    _FactoryMap::const_iterator found =
        _factoryMap.find(arg->type.GetTypeName());

    if (found == _factoryMap.end()) {
        TF_WARN(
            "Could not find CodeProcPrimvarValueDataSource for argument %s ",
            name.GetText());
        return _inputDs->Get(name);
    }

    if (auto primvarContainer =
            HdContainerDataSource::Cast(_inputDs->Get(name))) {
        _DataSourceFactory factory = found->second;

        // TfType argType = arg->type;
        SdfPath argPrimPath = arg->primvarPath.GetAbsoluteRootOrPrimPath();

        if (arg->IsFlags(Deformer::ArgumentFlags::readable)) {
            return _inputDs->Get(name);
        }
        if (arg->HasFlags(Deformer::ArgumentFlags::writable)) {
            return CodeProcPrimvarOverrideDataSource::New(
                primvarContainer,
                factory(*arg, _deformer, HdPrimvarSchemaTokens->vertex,
                        HdPrimvarSchemaTokens->point),
                HdPrimvarSchemaTokens->vertex, HdPrimvarSchemaTokens->point);
        }
    }
    return _inputDs->Get(name);
}

/* ----------------------- CodeProcDataSourceContainer ---------------------- */

TfTokenVector CodeProcDataSourceContainer::GetNames() {
    if (!_primDs) {
        return {};
    }
    return _primDs->GetNames();
}

HdDataSourceBaseHandle CodeProcDataSourceContainer::Get(const TfToken &name) {
    auto inputDs = _primDs->Get(name);

    if (name == HdPrimvarsSchemaTokens->primvars) {
        HdPrimvarsSchema primvarSchema =
            HdPrimvarsSchema::GetFromParent(_primDs);
        if (auto primvarContainer = HdContainerDataSource::Cast(inputDs)) {
            return CodeProcPrimvarsOverrideDataSource::New(
                _primPath, primvarContainer, primvarSchema, _deformer);
        }
    }
    return inputDs;
}

PXR_NAMESPACE_CLOSE_SCOPE