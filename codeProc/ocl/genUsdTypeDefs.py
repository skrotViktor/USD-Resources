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
            lines.append(f"#ifndef {support[key]}")
            lines += value
            lines.append(f"#endif")
        else:
            lines += value

    return lines


def linesFromSpecs(specs, isMatrix=False):
    typedefs = defaultdict(list)
    for s in specs:
        for spec in s['specs']:
            oclType = spec["SCL"]
            if "half" in oclType.lower():
                oclType = "half"
            
            nameFunc = filter(lambda x: "name" in x.lower(), spec)

            if isMatrix:
                oclType+=str(spec["DIM"])

            dim = spec.get("DIM")
            if not dim:
                typename = spec[next(nameFunc)](spec["SCL"])
                typedefs[oclType].append(f"typedef {oclType} {typename}[4];")
            else:
                typename = spec[next(nameFunc)](dim, spec["SCL"])
                typedefs[oclType].append(f"typedef {oclType} {typename}[{spec['DIM']}];")

    return linesFromDict(typedefs)

def writeOclTypes(path):
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

    lines.append("// TfToken")
    lines.append("typedef size_t TfToken")

    with open(path, "w") as f:
        f.writelines(line + "\n" for line in lines)
    
if __name__ == "__main__":
    ap = ArgumentParser(
        description='typedef common usd types for use in OpenCL.')
    ap.add_argument('--dst', default=os.path.join(os.curdir, "usdTypes.h"), help="Destination file path")
    ap.add_argument('--gfGenCode', help="Path to usd install gf/gdGenCode.py script")
    args = ap.parse_args()

    # Import gdGenCode.py from usd source
    spec = importlib.util.spec_from_file_location("gfGenCode", args.gfGenCode)
    gfGenCode = importlib.util.module_from_spec(spec)
    sys.modules["gfGenCode"] = gfGenCode
    spec.loader.exec_module(gfGenCode)

    writeOclTypes(os.path.join(os.path.dirname(__file__), "usdTypes.h"))