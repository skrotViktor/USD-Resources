#ifndef CODEPROC_PROCEDURALAPIADAPTER_H
#define CODEPROC_PROCEDURALAPIADAPTER_H

#include "api.h"
#include "pxr/pxr.h"
#include "pxr/usdImaging/usdImaging/apiSchemaAdapter.h"

PXR_NAMESPACE_OPEN_SCOPE

class CodeProcCodeProceduralAPIAdapter : public UsdImagingAPISchemaAdapter {
  public:
    CODEPROC_API
    ~CodeProcCodeProceduralAPIAdapter() override;

    using BaseAdapter = UsdImagingAPISchemaAdapter;

    CODEPROC_API
    HdContainerDataSourceHandle GetImagingSubprimData(
        UsdPrim const &prim, TfToken const &subprim,
        TfToken const &appliedInstanceName,
        const UsdImagingDataSourceStageGlobals &stageGlobals) override;

    CODEPROC_API
    HdDataSourceLocatorSet InvalidateImagingSubprim(
        UsdPrim const &prim, TfToken const &subprim,
        TfToken const &appliedInstanceName, TfTokenVector const &properties,
        UsdImagingPropertyInvalidationType invalidationType) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
