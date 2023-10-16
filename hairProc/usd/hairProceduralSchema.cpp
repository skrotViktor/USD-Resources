#include "pxr/imaging/hd/retainedDataSource.h"

#include "hairProceduralSchema.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HairProcHairProceduralSchemaTokens, HAIRPROC_SCHEMA_TOKENS);

HdVec2fArrayDataSourceHandle HairProcHairProceduralSchema::GetParamuv() {
    return _GetTypedDataSource<HdVec2fArrayDataSource>(HairProcHairProceduralSchemaTokens->paramuv);
}

HdPathArrayDataSourceHandle HairProcHairProceduralSchema::GetTarget() {
    return _GetTypedDataSource<HdPathArrayDataSource>(HairProcHairProceduralSchemaTokens->target);
}

HdIntArrayDataSourceHandle HairProcHairProceduralSchema::GetPrim() {
    return _GetTypedDataSource<HdIntArrayDataSource>(HairProcHairProceduralSchemaTokens->prim);
}

HdVec3fArrayDataSourceHandle HairProcHairProceduralSchema::GetRest() {
    return _GetTypedDataSource<HdVec3fArrayDataSource>(HairProcHairProceduralSchemaTokens->rest);
}

HdContainerDataSourceHandle HairProcHairProceduralSchema::BuildRetained(
        const HdVec2fArrayDataSourceHandle& paramuv,
        const HdPathArrayDataSourceHandle& target,
        const HdIntArrayDataSourceHandle& prim,
        const HdVec3fArrayDataSourceHandle& rest) {

    TfToken names[4];
    HdDataSourceBaseHandle values[4];

    size_t count = 0;
    if (paramuv) {
        names[count] = HairProcHairProceduralSchemaTokens->paramuv;
        values[count++] = paramuv;
    }
    if (target) {
        names[count] = HairProcHairProceduralSchemaTokens->target;
        values[count++] = target;
    }
    if (prim) {
        names[count] = HairProcHairProceduralSchemaTokens->prim;
        values[count++] = prim;
    }
    if (rest) {
        names[count] = HairProcHairProceduralSchemaTokens->rest;
        values[count++] = prim;
    }
    return HdRetainedContainerDataSource::New(count, names, values);
}

const TfToken& HairProcHairProceduralSchema::GetSchemaToken() {
    return HairProcHairProceduralSchemaTokens->hairProcedural;
}

HairProcHairProceduralSchema
HairProcHairProceduralSchema::GetFromParent(const HdContainerDataSourceHandle& parent) {    
    return HairProcHairProceduralSchema(
        parent 
        ? HdContainerDataSource::Cast(parent->Get(HairProcHairProceduralSchemaTokens->hairProcedural))
        : nullptr);
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetDefaultLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetPrimLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->prim);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetParamuvLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->paramuv);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetTargetLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->target);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetRestLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->rest);
    return locator;
}

HairProcHairProceduralSchema::Builder&
HairProcHairProceduralSchema::Builder::SetParamuv(const HdVec2fArrayDataSourceHandle& uv) {
    _paramuv = uv;
    return *this;
}

HairProcHairProceduralSchema::Builder&
HairProcHairProceduralSchema::Builder::SetTarget(const HdPathArrayDataSourceHandle& target) {
    _target = target;
    return *this;
}

HairProcHairProceduralSchema::Builder&
HairProcHairProceduralSchema::Builder::SetPrim(const HdIntArrayDataSourceHandle& prim) {
    _prim = prim;
    return *this;
}

HairProcHairProceduralSchema::Builder&
HairProcHairProceduralSchema::Builder::SetRest(const HdVec3fArrayDataSourceHandle& rest) {
    _rest = rest;
    return *this;
}

HdContainerDataSourceHandle HairProcHairProceduralSchema::Builder::Build() {
    return HairProcHairProceduralSchema::BuildRetained(
        _paramuv,
        _target,
        _prim,
        _rest
    );
}


PXR_NAMESPACE_CLOSE_SCOPE