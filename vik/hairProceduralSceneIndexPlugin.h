#ifndef HAIRPROC_PROCEDURALSCENEINDEX_PLUGIN_H
#define HAIRPROC_PROCEDURALSCENEINDEX_PLUGIN_H

#include "./tokens.h"

#include "pxr/pxr.h"
#include "pxr/imaging/hd/sceneIndexPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE

class HairProcHairProceduralSceneIndexPlugin : public HdSceneIndexPlugin {
public:
    HairProcHairProceduralSceneIndexPlugin();
    ~HairProcHairProceduralSceneIndexPlugin() override;

protected:
    HdSceneIndexBaseRefPtr _AppendSceneIndex(
        const HdSceneIndexBaseRefPtr& inputScene,
        const HdContainerDataSourceHandle& inputArgs) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
