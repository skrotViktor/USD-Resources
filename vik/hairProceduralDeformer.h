#ifndef HAIRPROC_PROCEDURAL_DEFORMER
#define HAIRPROC_PROCEDURAL_DEFORMER

#include "pxr/pxr.h"
#include "pxr/base/tf/declarePtrs.h"
#include "pxr/base/vt/value.h"
#include "pxr/imaging/hd/basisCurvesSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"

#include "hairProceduralSchema.h"

#include "api.h"


PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_REF_PTRS(HairProcHairProceduralDeformer);

class HairProcHairProceduralDeformer {
public:
    HairProcHairProceduralDeformer(VtArray<SdfPath> target, VtVec3fArray up, VtVec2fArray uv, VtIntArray prim);
    ~HairProcHairProceduralDeformer(){}

    VtVec3fArray deform(const VtVec3fArray& pts) {
        std::cout<< "DEFORM" << std::endl;
        // std::cout<< "target: " << _target.size()<<std::endl;
        // std::cout<< "up: " << _up.size()<<std::endl;
        // std::cout<< "uv: " << _uv.size()<<std::endl;
        // std::cout<< "prim: " << _prim.size()<<std::endl;

        return pts;
    }

private:
    VtArray<SdfPath> _target;
    VtVec3fArray _up;
    VtVec2fArray _uv;
    VtIntArray _prim;
};

using HairProcHairProceduralDeformerSharedPtr = std::shared_ptr<class HairProcHairProceduralDeformer>;


PXR_NAMESPACE_CLOSE_SCOPE

#endif