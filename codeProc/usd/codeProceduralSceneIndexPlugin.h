#ifndef CODEPROC_PROCEDURAL_SCENEINDEX_PLUGIN_H
#define CODEPROC_PROCEDURAL_SCENEINDEX_PLUGIN_H

#include "pxr/imaging/hd/sceneIndexPlugin.h"
#include "pxr/pxr.h"

PXR_NAMESPACE_OPEN_SCOPE

class CodeProcCodeProceduralSceneIndexPlugin : public HdSceneIndexPlugin {
  public:
    CodeProcCodeProceduralSceneIndexPlugin();
    ~CodeProcCodeProceduralSceneIndexPlugin() override;

  protected:
    HdSceneIndexBaseRefPtr
    _AppendSceneIndex(const HdSceneIndexBaseRefPtr &inputScene,
                      const HdContainerDataSourceHandle &inputArgs) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
