import importlib.util
import sys
import os
from collections import defaultdict
from argparse import ArgumentParser

# Script to add typedefines for common usd data types in opencl

def linesFromDict(data):
    support = {"double": "cl_khr_fp64", "half": "cl_khr_fp16"}

    lines = []
    for key, value in data.items():
        if key in support.keys():
            lines.append(f"#ifdef {support[key]}")
            lines += value
            lines.append(f"#endif")
        else:
            lines += value

    return lines


def getSpecPxrTypename(spec):
    nameFunc = filter(lambda x: "name" in x.lower(), spec)

    dim = spec.get("DIM")
    if not dim:
        return spec[next(nameFunc)](spec["SCL"])
    else:
        return spec[next(nameFunc)](dim, spec["SCL"])

def getSpecOclTypename(spec, vectorComponent=True):
    oclType = spec["SCL"]
    if "half" in oclType.lower():
        oclType = "half"

    if not vectorComponent:
        return oclType

    dim = spec.get("DIM", 4)
    oclType += str(dim)
    return oclType


def linesFromSpecs(specs, isMatrix=False):
    typedefs = defaultdict(list)
    for s in specs:
        for spec in s['specs']:

            oclBaseType = getSpecOclTypename(spec, False)
            oclVecType = getSpecOclTypename(spec)
            typename = getSpecPxrTypename(spec)
            
            if isMatrix:
                typename+=f"[{spec['DIM']}]"

            typedefs[oclBaseType].append(f"typedef {oclVecType} {typename};")

    return linesFromDict(typedefs)


def writeOclTypes(file):
    lines = []
    
    lines.append("// GfVec")
    lines += linesFromSpecs([gfGenCode.GetVecSpecs()])
    
    lines.append("// GfMatrix")
    lines += linesFromSpecs([gfGenCode.GetMatrix2Specs(), 
                             gfGenCode.GetMatrix3Specs(), 
                             gfGenCode.GetMatrix4Specs()],
                             isMatrix=True)

    lines.append("// GfQuat")
    lines += linesFromSpecs([gfGenCode.GetQuatSpecs()])

    file.writelines(line + "\n" for line in lines)


def writeOclLoad(file):
    vecTemplate = """static {typename} Ld{typename} (const int idx, const __global {type} *data) {{
    return ({typename})({constr});
}}"""
    matTemplate = """static void Ld{typename} (const int idx, const __global {type} *data, {typename} out) {{
    {constr}
}}"""
    specs = [gfGenCode.GetVecSpecs(),
             gfGenCode.GetMatrix2Specs(),
             gfGenCode.GetMatrix3Specs(),
             gfGenCode.GetMatrix4Specs()
             ]

    lines = ["\n// LOAD FUNCTIONS"]
    functiondefs = defaultdict(list)

    for spectype in specs:
        for spec in spectype.get("specs"):
            expr = []
            dim = spec["DIM"]
            typename = getSpecPxrTypename(spec)
            oclTypename = getSpecOclTypename(spec, False)
            if spec.get("MAT"):
                pxrTypename = gfGenCode.VecName(dim, spec["SCL"])
                for i in range(dim):
                    expr.append(f"out[{i}] = Ld{pxrTypename}(idx * {dim} + {i*dim}, data);")
                functiondefs[oclTypename].append(matTemplate.format(typename=typename, type=oclTypename, constr="\n\t".join(expr)))
            else:
                for i in range(dim):
                    expr.append(f"data[idx * {dim} + {i}]")
                functiondefs[oclTypename].append(vecTemplate.format(typename=typename, type=oclTypename, constr=",".join(expr)))

    lines += linesFromDict(functiondefs)

    file.writelines(line + "\n" for line in lines)


def writeOclStore(file):
    template = """static void St{typename} (const {typename} val, const int idx, __global {type} *buffer) {{
    {store}
}}"""
    specs = [gfGenCode.GetVecSpecs(),
             gfGenCode.GetMatrix2Specs(),
             gfGenCode.GetMatrix3Specs(),
             gfGenCode.GetMatrix4Specs()
             ]

    lines = ["\n// STORE FUNCTIONS"]
    functiondefs = defaultdict(list)

    for spectype in specs:
        for spec in spectype.get("specs"):
            store = []
            dim = spec["DIM"]
            typename = getSpecPxrTypename(spec)
            oclTypename = getSpecOclTypename(spec, False)
            vec = ["x", "y", "z", "w"]

            if spec.get("MAT"):
                pxrTypename = gfGenCode.VecName(dim, spec["SCL"])
                for i in range(dim):
                    store.append(f"St{pxrTypename}(val[{i}], idx * {dim} + {i*dim}, buffer);")
                functiondefs[oclTypename].append(template.format(typename=typename, type=oclTypename, store="\n\t".join(store)))
            else:
                for i in range(dim):
                    store.append(f"buffer[idx * {dim} + {i}] = val.{vec[i]};")
                functiondefs[oclTypename].append(template.format(typename=typename, type=oclTypename, store="\n\t".join(store)))

    lines += linesFromDict(functiondefs)

    file.writelines(line + "\n" for line in lines)


def WriteFile(path):
    file = open(path, "w")

    file.writelines(["#ifndef USD_TYPES_H\n", "#define USD_TYPES_H\n"])
    writeOclTypes(file)
    writeOclLoad(file)
    writeOclStore(file)
    file.writelines(["#endif //USD_TYPES_H"])

    file.close()


if __name__ == "__main__":
    ap = ArgumentParser(
        description='typedef common usd types for use in OpenCL.')
    ap.add_argument('--dst', default=os.path.join(os.curdir, "usdTypes.h"), help="Destination file path")
    ap.add_argument('--gfGenCode', help="Path to usd source gf/gfGenCode.py script")
    args = ap.parse_args()

    # Import gfGenCode.py from usd source
    spec = importlib.util.spec_from_file_location("gfGenCode", args.gfGenCode)
    gfGenCode = importlib.util.module_from_spec(spec)
    sys.modules["gfGenCode"] = gfGenCode
    spec.loader.exec_module(gfGenCode)

    path = os.path.join(os.path.dirname(__file__), "usdTypes.h")
    WriteFile(path)
