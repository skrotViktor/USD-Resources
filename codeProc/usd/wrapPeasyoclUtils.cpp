
#include "pxr/usd/sdf/ValueTypeName.h"
#include "pxr/pxr.h"

#include <boost/python/def.hpp>
#include <boost/python/tuple.hpp>

#include "peasyocl/Context.h"
#include "peasyocl/KernelUtils.h"

#include "./peacyoclUtils.h"

#include "stdint.h"
#include "string"

// PXR_NAMESPACE_USING_DIRECTIVE


// std::string _testFunction(TfToken token){
//     return testFunction(token);
// }



// void wrapPeasyoclUtils() {
    // boost::python::def("sizeFromToken", peasyoclUtils::oclSizeFromToken, (boost::python::arg("token")=TfToken()), "TEST FUNCTION DOCSTRING");
    // boost::python::def("typeFromToken", peasyoclUtils::oclTypeFromToken, (boost::python::arg("token")=TfToken()), "TEST FUNCTION DOCSTRING");
    // boost::python::def("generateKernelCode", peasyoclUtils::generateKernelCode, (boost::python::arg("data")=VtDictionary()), "TEST FUNCTION DOCSTRING");
    // boost::python::def("stringFromTfType", peasyoclUtils::oclStringFromTfType, (boost::python::arg("token")=TfType::GetUnknownType()), "TEST FUNCTION DOCSTRING");
    // boost::python::def("peasyoclTest", _testFunction, (boost::python::arg("token")=TfToken()), "TEST FUNCTION DOCSTRING");

    // def("GetMajorVersion", _GetMajorVersion,
    // _MakeVersionFuncDocstring("major", "int").c_str()); 
// }
