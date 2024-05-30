#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

#include "codeProceduralSceneIndex.h"
#include "codeProceduralSceneIndexPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(_tokens, ((sceneIndexPluginName,
                                    "CodeProcCodeProceduralSceneIndexPlugin")));

TF_REGISTRY_FUNCTION(TfType) {
  HdSceneIndexPluginRegistry::Define<CodeProcCodeProceduralSceneIndexPlugin>();
}

TF_REGISTRY_FUNCTION(HdSceneIndexPlugin) {
  HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
      TfToken(), _tokens->sceneIndexPluginName, nullptr, 0,
      HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

CodeProcCodeProceduralSceneIndexPlugin::
    CodeProcCodeProceduralSceneIndexPlugin() = default;
CodeProcCodeProceduralSceneIndexPlugin::
    ~CodeProcCodeProceduralSceneIndexPlugin() = default;

HdSceneIndexBaseRefPtr
CodeProcCodeProceduralSceneIndexPlugin::_AppendSceneIndex(
    const HdSceneIndexBaseRefPtr &inputSceneIndex,
    const HdContainerDataSourceHandle &inputArgs) {

  TF_UNUSED(inputArgs);
  return CodeProcCodeProceduralSceneIndex::New(inputSceneIndex);
}

PXR_NAMESPACE_CLOSE_SCOPE
