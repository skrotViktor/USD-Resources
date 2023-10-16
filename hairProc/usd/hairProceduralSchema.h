#ifndef HAIRPROC_PROCEDURAL_HYDRA_SCHEMA_H
#define HAIRPROC_PROCEDURAL_HYDRA_SCHEMA_H

#include "./api.h"
#include "./tokens.h"
#include "pxr/imaging/hd/schema.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

#define HAIRPROC_SCHEMA_TOKENS \
    (hairProcedural) \
    (target) \
    (prim) \
    (paramuv) \
    (rest) \

TF_DECLARE_PUBLIC_TOKENS(HairProcHairProceduralSchemaTokens, HAIRPROC_API, HAIRPROC_SCHEMA_TOKENS);

class HairProcHairProceduralSchema : public HdSchema {
public:
    HairProcHairProceduralSchema(HdContainerDataSourceHandle container) : HdSchema(container) {}

    HAIRPROC_API
    HdVec2fArrayDataSourceHandle GetParamuv();

    HAIRPROC_API
    HdPathArrayDataSourceHandle GetTarget();

    HAIRPROC_API
    HdIntArrayDataSourceHandle GetPrim();
    
    HAIRPROC_API
    HdVec3fArrayDataSourceHandle GetRest();

    HAIRPROC_API
    static HairProcHairProceduralSchema GetFromParent(const HdContainerDataSourceHandle& parent);

    HAIRPROC_API
    static const TfToken& GetSchemaToken();

    HAIRPROC_API
    static const HdDataSourceLocator& GetDefaultLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetParamuvLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetTargetLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetPrimLocator();
    
    HAIRPROC_API
    static const HdDataSourceLocator& GetRestLocator();

    HAIRPROC_API
    static HdContainerDataSourceHandle BuildRetained(
        const HdVec2fArrayDataSourceHandle& uv,
        const HdPathArrayDataSourceHandle& target,
        const HdIntArrayDataSourceHandle& prim,
        const HdVec3fArrayDataSourceHandle& rest);

    class Builder {
    public:
        HAIRPROC_API
        Builder& SetParamuv(const HdVec2fArrayDataSourceHandle& uv);

        HAIRPROC_API
        Builder& SetTarget(const HdPathArrayDataSourceHandle& target);

        HAIRPROC_API
        Builder& SetPrim(const HdIntArrayDataSourceHandle& prim);
        
        HAIRPROC_API
        Builder& SetRest(const HdVec3fArrayDataSourceHandle& rest);
    
        HAIRPROC_API
        HdContainerDataSourceHandle Build();

    private:
        HdVec2fArrayDataSourceHandle _paramuv;
        HdPathArrayDataSourceHandle _target;
        HdIntArrayDataSourceHandle _prim;
        HdVec3fArrayDataSourceHandle _rest;
    };
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif