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
    (up) \
    (uv) \

TF_DECLARE_PUBLIC_TOKENS(HairProcHairProceduralSchemaTokens, HAIRPROC_API, HAIRPROC_SCHEMA_TOKENS);

class HairProcHairProceduralSchema : public HdSchema {
public:
    HairProcHairProceduralSchema(HdContainerDataSourceHandle container) : HdSchema(container) {}

    HAIRPROC_API
    HdVec2fArrayDataSourceHandle GetUv();

    HAIRPROC_API
    HdPathArrayDataSourceHandle GetTarget();

    HAIRPROC_API
    HdIntArrayDataSourceHandle GetPrim();

    HAIRPROC_API
    HdVec3fArrayDataSourceHandle GetUp();

    HAIRPROC_API
    static HairProcHairProceduralSchema GetFromParent(const HdContainerDataSourceHandle& parent);

    HAIRPROC_API
    static const TfToken& GetSchemaToken();

    HAIRPROC_API
    static const HdDataSourceLocator& GetDefaultLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetUvLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetTargetLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetPrimLocator();

    HAIRPROC_API
    static const HdDataSourceLocator& GetUpLocator();

    HAIRPROC_API
    static HdContainerDataSourceHandle BuildRetained(
        const HdVec2fArrayDataSourceHandle& uv,
        const HdPathArrayDataSourceHandle& target,
        const HdIntArrayDataSourceHandle& prim,
        const HdVec3fArrayDataSourceHandle& up);

    class Builder {
    public:
        HAIRPROC_API
        Builder& SetUv(const HdVec2fArrayDataSourceHandle& uv);

        HAIRPROC_API
        Builder& SetTarget(const HdPathArrayDataSourceHandle& target);

        HAIRPROC_API
        Builder& SetPrim(const HdIntArrayDataSourceHandle& prim);

        HAIRPROC_API
        Builder& SetUp(const HdVec3fArrayDataSourceHandle& up);
    
        HAIRPROC_API
        HdContainerDataSourceHandle Build();

    private:
        HdVec2fArrayDataSourceHandle _uv;
        HdPathArrayDataSourceHandle _target;
        HdIntArrayDataSourceHandle _prim;
        HdVec3fArrayDataSourceHandle _up;
    };
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif