
#include "pxr/base/tf/stringUtils.h"
#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/usdImaging/usdImaging/dataSourceAttribute.h"

#include "hairProceduralAPIAdapter.h"
#include "hairProceduralAPI.h"

#include "pxr/base/tf/debug.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (target)
    (prim)
    (up)
);

TF_REGISTRY_FUNCTION(TfType) {
    typedef HairProcHairProceduralAPIAdapter Adapter;
    TfType t = TfType::Define<Adapter, TfType::Bases<Adapter::BaseAdapter>>();
    t.SetFactory<UsdImagingAPISchemaAdapterFactory<Adapter>>();
}

HdContainerDataSourceHandle
HairProcHairProceduralAPIAdapter::GetImagingSubprimData(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        const UsdImagingDataSourceStageGlobals& stageGlobals) {
    std::cout<<"HELLO FROM GetImagingSubprimData"<<std::endl;
    return nullptr;
}

HdDataSourceLocatorSet
HairProcHairProceduralAPIAdapter::InvalidateImagingSubprim(
        UsdPrim const& prim,
        TfToken const& subprim,
        TfToken const& appliedInstanceName,
        TfTokenVector const& properties) {
    
    std::cout<<"HELLO FROM InvalidateImagingSubprim"<<std::endl;
    return HdDataSourceLocatorSet();
}

PXR_NAMESPACE_CLOSE_SCOPE