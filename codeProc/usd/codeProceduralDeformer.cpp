#include "codeProceduralDeformer.h"
#include "codeProceduralDeformerArguments.h"
#include "peasyocl/Context.h"
#include "pxr/base/trace/trace.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace Deformer {

CodeProcCodeProceduralDeformer::CodeProcCodeProceduralDeformer() { InitOCL(); }

const int CodeProcCodeProceduralDeformer::InitOCL() {
    TRACE_FUNCTION();

    int err;
    peasyocl::Context *_oclContext = peasyocl::Context::GetInstance();
    err = _oclContext->Init();
    return err;
}

const int CodeProcCodeProceduralDeformer::Update(const bool &force) {
    if (!_isDirty && !force) {
        return 0;
    }
    return 0;
}

void CodeProcCodeProceduralDeformer::AddArgument(const Argument &arg,
                                                 const VtValue &val) {
    size_t size = arg.arraySize * arg.dataSize;
    std::string name = arg.name.GetString();

    // if (arg.arraySize > 1) {
    //     std::vector<int> ids;
    //     std::vector<int> lens;
    //     ids.resize(arg.arraySize);
    //     // std::iota(ids.begin(), ids.end(), 0);
    
    //     lens.resize(arg.arraySize);
    //     // std::fill(lens.begin(), ids.end(), 4);

    //     _kernelHandle->AddArgument(CL_MEM_READ_ONLY,
    //                                std::string(name + "_indices").c_str(), arg.arraySize * sizeof(int),
    //                                HdGetValueData(val));
    //     _kernelHandle->AddArgument(CL_MEM_READ_ONLY,
    //                                std::string(name + "_lengths").c_str(), arg.arraySize * sizeof(int),
    //                                HdGetValueData(val));
    // }

    // std::cout << sizeof(val) << "->" << arg.size << "->" << sizeof(float) << std::endl;
    std::cout << arg.arraySize << "->" << arg.vectorSize << "->" << arg.dataSize << std::endl;
    std::cout << val << std::endl;
    // std::cout << val.GetElementTypeid().name() << std::endl;

    if (arg.HasFlags(ArgumentFlags::writable)) {
        _kernelHandle->AddArgument(CL_MEM_READ_WRITE, name.c_str(), size,
                                   HdGetValueData(val));
    } 
    else if (arg.IsFlags(ArgumentFlags::readable)) {
        _kernelHandle->AddArgument(CL_MEM_READ_ONLY, name.c_str(), size,
                                   HdGetValueData(val));
    }
    _isDirty = true;
    _arguments.insert(arg);
}

void CodeProcCodeProceduralDeformer::SetArguments(const ArgumentSet &args) {}

void CodeProcCodeProceduralDeformer::SetWorkgroupSize(const int &size) {
    _kernelHandle->AddArgument(CL_MEM_READ_ONLY, "globalSize", sizeof(int),
                               size);
    _workgroupSize = size;
}

void CodeProcCodeProceduralDeformer::SetKernelCode(const std::string &code) {
    _kernelCode = code;
    peasyocl::Context *_oclContext = peasyocl::Context::GetInstance();
    _oclContext->AddSource(_kernelCode);
    BuildClProgram();
}

} // namespace Deformer

PXR_NAMESPACE_CLOSE_SCOPE