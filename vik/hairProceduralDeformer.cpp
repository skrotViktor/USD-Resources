#include "hairProceduralDeformer.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE


HairProcHairProceduralDeformer::HairProcHairProceduralDeformer(
        VtArray<SdfPath> target,
        VtVec3fArray up,
        VtVec2fArray uv,
        VtIntArray prim) : _target(target), _up(up), _uv(uv), _prim(prim) {}


PXR_NAMESPACE_CLOSE_SCOPE