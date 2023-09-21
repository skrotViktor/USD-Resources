#ifndef HAIRPROC_PROCEDURALAPIADAPTER_H
#define HAIRPROC_PROCEDURALAPIADAPTER_H

#include "pxr/usdImaging/usdImaging/apiSchemaAdapter.h"

#include "./api.h"


PXR_NAMESPACE_OPEN_SCOPE

class HairProcHairProceduralAPIAdapter : public UsdImagingAPISchemaAdapter {
public:
    using BaseAdapter = UsdImagingAPISchemaAdapter;

    HAIRPROC_API
    HdContainerDataSourceHandle GetImagingSubprimData(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        const UsdImagingDataSourceStageGlobals& stageGlobals) override;
    
    HAIRPROC_API
    HdDataSourceLocatorSet InvalidateImagingSubprim(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        TfTokenVector const& properties) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
