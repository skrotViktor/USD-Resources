
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

#include "pxr/base/tf/envSetting.h"

#include "hairProceduralSceneIndexPlugin.h"
#include "hairProceduralSceneIndex.h"
#include <string>
#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS (
    _tokens,
    ((sceneIndexPluginName, "HairProcHairProceduralSceneIndexPlugin")));

TF_REGISTRY_FUNCTION(TfType) {
    HdSceneIndexPluginRegistry::Define<HairProcHairProceduralSceneIndexPlugin>();
}

TF_REGISTRY_FUNCTION(HdSceneIndexPlugin) {
    HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
        TfToken(),
        _tokens->sceneIndexPluginName,
        nullptr,
        0, // insertion phase
        HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

HairProcHairProceduralSceneIndexPlugin::HairProcHairProceduralSceneIndexPlugin() = default;
HairProcHairProceduralSceneIndexPlugin::~HairProcHairProceduralSceneIndexPlugin() = default;

HdSceneIndexBaseRefPtr
HairProcHairProceduralSceneIndexPlugin::_AppendSceneIndex(
        const HdSceneIndexBaseRefPtr& inputSceneIndex,
        const HdContainerDataSourceHandle& inputArgs) {
    
    TF_UNUSED(inputArgs);
    return HairProcHairProceduralSceneIndex::New(inputSceneIndex);
}

PXR_NAMESPACE_CLOSE_SCOPE
