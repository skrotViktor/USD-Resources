#ifndef HAIRPROC_PROCEDURALSCENEINDEX_H
#define HAIRPROC_PROCEDURALSCENEINDEX_H

#include "pxr/pxr.h"
#include "pxr/imaging/hd/filteringSceneIndex.h"
#include "pxr/usdImaging/usdImaging/stageSceneIndex.h"
#include "pxr/imaging/hd/primvarSchema.h"
#include "pxr/imaging/hd/meshSchema.h"

#include "./api.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(HairProcHairProceduralSceneIndex);

class HairProcHairProceduralSceneIndex : public HdSingleInputFilteringSceneIndexBase 
{
public:
    HAIRPROC_API
    static HairProcHairProceduralSceneIndexRefPtr New(
        const HdSceneIndexBaseRefPtr& inputSceneIdndex);

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
};

PXR_NAMESPACE_CLOSE_SCOPE
#endif
