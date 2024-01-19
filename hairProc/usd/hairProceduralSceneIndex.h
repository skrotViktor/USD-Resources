#ifndef HAIRPROC_PROCEDURAL_SCENEINDEX_H
#define HAIRPROC_PROCEDURAL_SCENEINDEX_H

#include "pxr/imaging/hd/filteringSceneIndex.h"

#include "hairProceduralDeformer.h"
#include "api.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(HairProcHairProceduralSceneIndex);

class HairProcHairProceduralSceneIndex final : public HdSingleInputFilteringSceneIndexBase 
{
public:

    HAIRPROC_API
    static HairProcHairProceduralSceneIndexRefPtr New(
        const HdSceneIndexBaseRefPtr& inputSceneIndex);

    HAIRPROC_API
    HdSceneIndexPrim GetPrim(const SdfPath& primPath) const override;

    HAIRPROC_API
    SdfPathVector GetChildPrimPaths(const SdfPath& primPath) const override;

protected:
    HairProcHairProceduralSceneIndex(const HdSceneIndexBaseRefPtr& inputSceneIndex);

    void _PrimsAdded(const HdSceneIndexBase& sender, const HdSceneIndexObserver::AddedPrimEntries& entries) override;

    void _PrimsRemoved(const HdSceneIndexBase& sender, const HdSceneIndexObserver::RemovedPrimEntries& entried) override;

    void _PrimsDirtied(const HdSceneIndexBase& sender, const HdSceneIndexObserver::DirtiedPrimEntries& entries) override;

private:
    void _init_deformer(
        const SdfPath& primPath,
        HairProcHairProceduralSchema& procSchema,
        HdBasisCurvesSchema& basisCurvesSchema,
        HdPrimvarsSchema& primvarSchema);

    typedef std::map<SdfPath, std::unordered_set<SdfPath, SdfPath::Hash>> _TargetsMap;
    mutable _TargetsMap _targets;

    typedef std::unordered_map<SdfPath, HairProcHairProceduralDeformerSharedPtr, SdfPath::Hash> _HairProcMap;
    mutable _HairProcMap _deformerMap;
};

PXR_NAMESPACE_CLOSE_SCOPE
#endif
