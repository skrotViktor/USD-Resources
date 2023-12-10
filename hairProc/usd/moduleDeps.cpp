#include "pxr/pxr.h"
#include "pxr/base/tf/registryManager.h"
#include "pxr/base/tf/scriptModuleLoader.h"
#include "pxr/base/tf/token.h"

#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfScriptModuleLoader) {
    // List of direct dependencies for this library.
    const std::vector<TfToken> reqs = {
        TfToken("sdf"),
        TfToken("tf"),
        TfToken("usd"),
        TfToken("vt"),
        TfToken("hd"),
        TfToken("usdImaging")
    };
#ifndef BUILD_HOUDINI_PLUGIN
    TfScriptModuleLoader::GetInstance().RegisterLibrary(TfToken("hairProc"), TfToken("vik.HairProc"), reqs);
#else
    TfScriptModuleLoader::GetInstance().RegisterLibrary(TfToken("hairProcHoudini"), TfToken("vik.HairProcHoudini"), reqs);
#endif

}

PXR_NAMESPACE_CLOSE_SCOPE
