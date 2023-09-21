#include "./hairProceduralSceneIndex.h"

#include "pxr/pxr.h"
#include "./tokens.h"
#include "pxr/imaging/hd/filteringSceneIndex.h"
#include "pxr/usdImaging/usdImaging/stageSceneIndex.h"
#include "pxr/imaging/hd/primvarSchema.h"
#include "pxr/imaging/hd/meshSchema.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

HairProcHairProceduralSceneIndexRefPtr
HairProcHairProceduralSceneIndex::New(const HdSceneIndexBaseRefPtr& inputSceneIndex, const HdContainerDataSourceHandle& inputArgs) {
    return TfCreateRefPtr(new HairProcHairProceduralSceneIndex(inputSceneIndex));
}

HairProcHairProceduralSceneIndex::HairProcHairProceduralSceneIndex(const HdSceneIndexBaseRefPtr& inputSceneIndex) 
        : HdSingleInputFilteringSceneIndexBase(inputSceneIndex) {
    std::cout<<"HELLO FROM SCENE INDEX CONSTR"<<std::endl;
}

HdSceneIndexPrim
HairProcHairProceduralSceneIndex::GetPrim(const SdfPath& primPath) const {
    std::cout<<"HELLO FROM GET PRIM: "<< primPath <<std::endl;
    HdSceneIndexPrim prim = _GetInputSceneIndex()->GetPrim(primPath);
    
}

SdfPathVector
HairProcHairProceduralSceneIndex::GetChildPrimPaths(const SdfPath& primPath) const {
    std::cout<<"HELLO FROM SCENE INDEX GET CHILD PATHS"<<std::endl;
    return _GetInputSceneIndex()->GetChildPrimPaths(primPath);
}

void 
HairProcHairProceduralSceneIndex::_PrimsAdded(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::AddedPrimEntries& entries) {
    std::cout<<"HELLO FROM SCENE INDEX _PRIMSADDED"<<std::endl;
    _SendPrimsAdded(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsRemoved(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::RemovedPrimEntries& entries) {
    std::cout<<"HELLO FROM SCENE INDEX PRIMSREMOVED"<<std::endl;
    _SendPrimsRemoved(entries);
}

void
HairProcHairProceduralSceneIndex::_PrimsDirtied(
        const HdSceneIndexBase& sender,
        const HdSceneIndexObserver::DirtiedPrimEntries& entries) {
    std::cout<<"HELLO FROM SCENE INDEX PRIMSDIRTIED"<<std::endl;
    _SendPrimsDirtied(entries);
}

PXR_NAMESPACE_CLOSE_SCOPE

