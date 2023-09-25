#ifndef HAIRPROC_PROCEDURALAPIADAPTER_H
#define HAIRPROC_PROCEDURALAPIADAPTER_H

#include "pxr/pxr.h"
#include "pxr/usdImaging/usdImaging/apiSchemaAdapter.h"

#include "api.h"

#include <iostream>


PXR_NAMESPACE_OPEN_SCOPE

class HairProcHairProceduralAPIAdapter : public UsdImagingAPISchemaAdapter {
public:
    HAIRPROC_API
    ~HairProcHairProceduralAPIAdapter() override;

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
        TfTokenVector const& properties,
        UsdImagingPropertyInvalidationType invalidationType) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
