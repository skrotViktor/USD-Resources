#ifndef CODEPROC_PROCEDURAL_SCENEINDEX_H
#define CODEPROC_PROCEDURAL_SCENEINDEX_H

#include "api.h"
#include "codeProceduralDeformer.h"
#include "codeProceduralSchema.h"

#include "pxr/imaging/hd/filteringSceneIndex.h"
#include "pxr/imaging/hd/primvarsSchema.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(CodeProcCodeProceduralSceneIndex);

class CodeProcCodeProceduralSceneIndex final
    : public HdSingleInputFilteringSceneIndexBase {
  public:
    CODEPROC_API
    static CodeProcCodeProceduralSceneIndexRefPtr
    New(const HdSceneIndexBaseRefPtr &inputSceneIndex);

    CODEPROC_API
    HdSceneIndexPrim GetPrim(const SdfPath &primPath) const override;

    CODEPROC_API
    SdfPathVector GetChildPrimPaths(const SdfPath &primPath) const override;

  protected:
    CodeProcCodeProceduralSceneIndex(
        const HdSceneIndexBaseRefPtr &inputSceneIndex);

    void
    _PrimsAdded(const HdSceneIndexBase &sender,
                const HdSceneIndexObserver::AddedPrimEntries &entries) override;

    void _PrimsRemoved(
        const HdSceneIndexBase &sender,
        const HdSceneIndexObserver::RemovedPrimEntries &entried) override;

    void _PrimsDirtied(
        const HdSceneIndexBase &sender,
        const HdSceneIndexObserver::DirtiedPrimEntries &entries) override;

  private:
    Deformer::Argument _GetArgFromPath(const SdfPath &primPath,
                                       const Deformer::ArgumentFlags &flags,
                                       const TfToken &name) const;
    VtValue _GetPrimvarValue(const SdfPath &primPath) const;

    void _init_deformer(const SdfPath &primPath,
                        CodeProcCodeProceduralSchema &procSchema,
                        HdPrimvarsSchema &primvarSchema);

    typedef std::unordered_map<
        SdfPath, Deformer::CodeProcCodeProceduralDeformerSharedPtr,
        SdfPath::Hash>
        _DeformerMap;

    _DeformerMap _deformerMap;
};

PXR_NAMESPACE_CLOSE_SCOPE
#endif
