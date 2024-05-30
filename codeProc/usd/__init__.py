from pxr import Tf
Tf.PreparePythonModule()
del Tf


class KernelUtils:
    templateKernel = \
"""// Defines
// Toggle support for double and half types. Set 'enable' or 'disable'
#pragma OPENCL EXTENSION cl_khr_fp16 : enable \n\
#pragma OPENCL EXTENSION cl_khr_fp64 : enable \n\
{}

// Includes
#include "usdTypes.h"
{}


__kernel void {} (
{}
) 
{{
    int idx = get_global_id(0);
    if (idx >= globalSize)
        return;
}}
"""
    def __init__(self, kernelName="myKernel"):
        self.kernelName = kernelName
        self.includes = []
        self.defines = {}

        self._argNames = []
        self._argTypes = []
        self._argIsArrays = []


    def addArg(self, argName, argType, isArray):
        self._argNames.append(argName)
        self._argTypes.append(argType)
        self._argIsArrays.append(isArray)


    def addArgFromAttribute(self, attribute, argName):
        tn = attribute.GetTypeName()
        argType = tn.scalarType.type.typeName
        argIsArray = tn.isArray
        self.addArg(argName, argType, argIsArray)


    def addInclude(self, moduleName):
        self.includes.append(moduleName)
    

    def addDefines(self, name, val):
        self.defines[name] = val
    

    def generateKernelCode(self):
        definesStr = ""
        for d in self.defines:
            definesStr += f"#ifndef {d} \n"
            definesStr += f"#define {d} \n"
            definesStr += f"#endif \n"
        
        includeStr = ""
        for i in self.includes:
            includeStr += f"#include {i} \n"

        indent = " " * len(f"__kernel void {self.kernelName}")
        args = f"{indent}const int globalSize,"
        for name, argtype, isArray in zip(self._argNames, self._argTypes, self._argIsArrays):
            if isArray:
                args += f"\n{indent}__global int* {name}_indices,"
                args += f"\n{indent}__global int* {name}_lengths,"

            args += f"\n{indent}__global {argtype}* {name},"
        
        args = args[:-1]

        return self.templateKernel.format(
            definesStr,
            includeStr,
            self.kernelName,
            args
        )


    

    
