
#ifndef PEASYOClUTILS_H
#define PEASYOClUTILS_H
 
#include <string>
// #include "pxr/base/tf/token.h"
#include "pxr/base/tf/type.h"
#include "pxr/base/vt/dictionary.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/vt/types.h"

#include "pxr/usd/sdf/schema.h"
#include "pxr/usd/sdf/valueTypeName.h"

// PXR_NAMESPACE_OPEN_SCOPE
using namespace pxr;

namespace peasyoclUtils{

template<typename T>
struct OclTypeName {
    static const std::string Get() {
        return typeid(T).name();
    }
};

inline std::string generateKernelCode(VtDictionary dict) {
    std::stringstream ss;
    
    for(auto i: dict) {
        std::cout<< i.first <<std::endl;
        std::cout<< i.second <<std::endl;
    }

    return ss.str();
}


inline std::string oclTypeFromToken(TfToken token){
    // TfType t = TfType::FindByName(token);
    SdfValueTypeName vtName = SdfSchema::GetInstance().FindType(token);

    VtValue val = vtName.GetScalarType().GetDefaultValue();
    auto typed = val.CastToTypeid(vtName.GetType().GetTypeid());
    std::cout<< typed << std::endl;
    // auto t = val.GetType().GetTypeName();
    // vtName.GetDimensions()
    // auto* typedVal = val.CastToTypeid(vtName.GetScalarType().GetType().GetTypeid());
    // VtValue* typedVal = val.CastToTypeid()
    // std::cout<< typedVal.ScalarType << std::endl;
    return val.GetType().GetTypeName();
}

inline int oclSizeFromToken(TfToken token){
    return token.size();
}

}

// PXR_NAMESPACE_CLOSE_SCOPE
#endif