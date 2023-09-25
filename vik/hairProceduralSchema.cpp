#include "pxr/imaging/hd/retainedDataSource.h"

#include "hairProceduralSchema.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE
// TF_DEFINE_PUBLIC_TOKENS(HairProcTokens, (hairProcedural));
TF_DEFINE_PUBLIC_TOKENS(HairProcHairProceduralSchemaTokens, HAIRPROC_SCHEMA_TOKENS);

HdVec2fArrayDataSourceHandle HairProcHairProceduralSchema::GetUv() {
    return _GetTypedDataSource<HdVec2fArrayDataSource>(HairProcHairProceduralSchemaTokens->uv);
}

HdPathArrayDataSourceHandle HairProcHairProceduralSchema::GetTarget() {
    return _GetTypedDataSource<HdPathArrayDataSource>(HairProcHairProceduralSchemaTokens->target);
}

HdIntArrayDataSourceHandle HairProcHairProceduralSchema::GetPrim() {
    return _GetTypedDataSource<HdIntArrayDataSource>(HairProcHairProceduralSchemaTokens->prim);
}

HdVec3fArrayDataSourceHandle HairProcHairProceduralSchema::GetUp() {
    return _GetTypedDataSource<HdVec3fArrayDataSource>(HairProcHairProceduralSchemaTokens->up);
}

HdContainerDataSourceHandle HairProcHairProceduralSchema::BuildRetained(
        const HdVec2fArrayDataSourceHandle& uv,
        const HdPathArrayDataSourceHandle& target,
        const HdIntArrayDataSourceHandle& prim,
        const HdVec3fArrayDataSourceHandle& up) {

    TfToken names[4];
    HdDataSourceBaseHandle values[4];

    size_t count = 0;
    if (uv) {
        names[count] = HairProcHairProceduralSchemaTokens->uv;
        values[count++] = uv;
    }
    if (target) {
        names[count] = HairProcHairProceduralSchemaTokens->target;
        values[count++] = target;
    }
    if (prim) {
        names[count] = HairProcHairProceduralSchemaTokens->prim;
        values[count++] = prim;
    }
    if (up) {
        names[count] = HairProcHairProceduralSchemaTokens->up;
        values[count++] = up;
    }
    return HdRetainedContainerDataSource::New(count, names, values);
}

const TfToken& HairProcHairProceduralSchema::GetSchemaToken() {
    return HairProcHairProceduralSchemaTokens->hairProcedural;
}

HairProcHairProceduralSchema
HairProcHairProceduralSchema::GetFromParent(const HdContainerDataSourceHandle& parent) {
    // std::cout<<parent->Get(HairProcTokens->HairProceduralAPI)<<std::endl;
    
    return HairProcHairProceduralSchema(
        parent 
        ? HdContainerDataSource::Cast(parent->Get(HairProcTokens->HairProceduralAPI))
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

const HdDataSourceLocator& HairProcHairProceduralSchema::GetUvLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->uv);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetTargetLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->target);
    return locator;
}

const HdDataSourceLocator& HairProcHairProceduralSchema::GetUpLocator() {
    static const HdDataSourceLocator locator(
        HairProcHairProceduralSchemaTokens->hairProcedural,
        HairProcHairProceduralSchemaTokens->up);
    return locator;
}

HairProcHairProceduralSchema::Builder&
HairProcHairProceduralSchema::Builder::SetUv(const HdVec2fArrayDataSourceHandle& uv) {
    _uv = uv;
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
HairProcHairProceduralSchema::Builder::SetUp(const HdVec3fArrayDataSourceHandle& up) {
    _up = up;
    return *this;
}

HdContainerDataSourceHandle HairProcHairProceduralSchema::Builder::Build() {
    return HairProcHairProceduralSchema::BuildRetained(
        _uv,
        _target,
        _prim,
        _up
    );
}


PXR_NAMESPACE_CLOSE_SCOPE