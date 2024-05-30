
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

#include "hairProceduralSceneIndex.h"
#include "hairProceduralSceneIndexPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(_tokens, ((sceneIndexPluginName,
                                    "HairProcHairProceduralSceneIndexPlugin")));

TF_REGISTRY_FUNCTION(TfType) {
    HdSceneIndexPluginRegistry::Define<
        HairProcHairProceduralSceneIndexPlugin>();
}

TF_REGISTRY_FUNCTION(HdSceneIndexPlugin) {
    HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
        TfToken(), _tokens->sceneIndexPluginName, nullptr, 0,
        HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

HairProcHairProceduralSceneIndexPlugin::
    HairProcHairProceduralSceneIndexPlugin() = default;
HairProcHairProceduralSceneIndexPlugin::
    ~HairProcHairProceduralSceneIndexPlugin() = default;

HdSceneIndexBaseRefPtr
HairProcHairProceduralSceneIndexPlugin::_AppendSceneIndex(
    const HdSceneIndexBaseRefPtr &inputSceneIndex,
    const HdContainerDataSourceHandle &inputArgs) {

    TF_UNUSED(inputArgs);
    return HairProcHairProceduralSceneIndex::New(inputSceneIndex);
}

PXR_NAMESPACE_CLOSE_SCOPE
