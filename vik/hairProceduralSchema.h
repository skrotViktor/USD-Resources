#ifndef HAIRPROC_PROCEDURAL_HYDRA_SCHEMA_H
#define HAIRPROC_PROCEDURAL_HYDRA_SCHEMA_H

#include "./api.h"
#include "pxr/imaging/hd/schema.h"


PXR_NAMESPACE_OPEN_SCOPE

#define HAIRPROCHAIRPROCEDURAL_SCHEMA_TOKENS \
    (hairProcedural) \
    (target) \
    (prim) \
    (up) \

TF_DECLARE_PUBLIC_TOKENS(HairProcHairProceduralSchemaTokens, HAIRPROC_API, HAIRPROCHAIRPROCEDURAL_SCHEMA_TOKENS);

class HairProcHairProceduralSchema : public HdSchema {
public:
    HairProcHairProceduralSchema(HdContainerDataSourceHandle& container) : HdSchema(container) {}

    HAIRPROC_API
    HdVec2fArrayDataSourceHandle GetUv();

    HAIRPROC_API
    HdPathArrayDataSourceHandle GetTarget();

    HAIRPROC_API
    HdIntArrayDataSourceHandle GetPrim();

    HAIRPROC_API
    static HairProcHairProceduralSchema GetFromParent(const HdContainerDataSourceAtomicHandle& parent);

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
    static HdContainerDataSourceHandle BuildRetained(
        const HdVec2fArrayDataSourceHandle& uv,
        const HdPathArrayDataSourceHandle& target,
        const HdIntArrayDataSourceHandle& prim
    );

    class Builder {
    public:
        HAIRPROC_API
        Builder& SetUv(const HdVec2fArrayDataSourceHandle& uv);
        
        HAIRPROC_API
        Builder& SetTarget(const HdPathArrayDataSourceHandle& target);
        
        HAIRPROC_API
        Builder& SetPrim(const HdIntArrayDataSourceHandle& prim);
        
        HAIRPROC_API
        HdContainerDataSourceHandle Build();

    private:
        HdVec2fArrayDataSourceHandle _uv;
        HdPathArrayDataSourceHandle _target;
        HdIntArrayDataSourceHandle _prim;
    };
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif