
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

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
    const HdSceneIndexPluginRegistry::InsertionPhase insertionPhase = 1;

    HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
        "", _tokens->sceneIndexPluginName, nullptr, insertionPhase, HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

HairProcHairProceduralSceneIndexPlugin::HairProcHairProceduralSceneIndexPlugin() = default;
// HairProcHairProceduralSceneIndexPlugin::~HairProcHairProceduralSceneIndexPlugin() = default;

HdSceneIndexBaseRefPtr
HairProcHairProceduralSceneIndexPlugin::_AppendSceneIndex(
        const HdSceneIndexBaseRefPtr& inputSceneIndex,
        const HdContainerDataSourceHandle& inputArgs) {
    
    TF_UNUSED(inputArgs);
    std::cout<<"HELLO FROM APPEND" <<std::endl;
    return HairProcHairProceduralSceneIndex::New(inputSceneIndex, inputArgs);
}

PXR_NAMESPACE_CLOSE_SCOPE
