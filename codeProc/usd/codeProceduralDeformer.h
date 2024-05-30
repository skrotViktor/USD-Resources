#ifndef CODEPROC_PROCEDURAL_DEFORMER
#define CODEPROC_PROCEDURAL_DEFORMER

#include "codeProceduralDeformerArguments.h"
#include "peasyocl/Context.h"
#include "pxr/base/vt/traits.h"
#include <string_view>

#include "pxr/base/gf/vec3f.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace Deformer {

struct Argument;
using ArgumentSet = std::set<Argument>;

class CodeProcCodeProceduralDeformer {
  public:
    CodeProcCodeProceduralDeformer();

    const bool IsDirty() const { return _isDirty; };

    const int InitOCL();
    const int Update(const bool &force = false);

    void Finish() const {
        peasyocl::Context *ctx = peasyocl::Context::GetInstance();
        ctx->Finish();
    }

    template <typename T> T GetSolvedValue(const Argument &arg) const;

    void SetKernelCode(const std::string &code);
    const std::string_view GetKernelCode() { return _kernelCode; }

    void SetWorkgroupSize(const int &size);
    const int GetWorkgroupSize() { return _workgroupSize; }

    const ArgumentSet GetArguments() const { return _arguments; }
    void SetArguments(const ArgumentSet &args);
    void AddArgument(const Argument &arg, const VtValue &val);

    template <typename T>
    ArgumentSet GetFilteredArguments(const T &filter) const {
        return _GetArguments(_arguments, filter);
    }

    void BuildClProgram() {
        peasyocl::Context *ctx = peasyocl::Context::GetInstance();
        ctx->Build();
        _kernelHandle = ctx->AddKernel("myKernel");
    }

    peasyocl::KernelHandle *GetKernelHandle() { return _kernelHandle; };

  private:
    mutable bool _isDirty = true;

    ArgumentSet _arguments;
    std::string _kernelCode;
    size_t _workgroupSize;
    peasyocl::KernelHandle *_kernelHandle;
};

using CodeProcCodeProceduralDeformerSharedPtr =
    std::shared_ptr<class CodeProcCodeProceduralDeformer>;

template <typename T>
T CodeProcCodeProceduralDeformer::GetSolvedValue(const Argument &arg) const {
    peasyocl::Context *ctx = peasyocl::Context::GetInstance();
    int err;
    if (_kernelHandle->dirty) {
        err = ctx->Execute(_workgroupSize, _kernelHandle);
        if (err != CL_SUCCESS) {
            return {};
        }
    }

    T result;
    if constexpr (VtIsArray<T>()) {
        result.resize(arg.arraySize);
        err = _kernelHandle->ReadBufferData(result.begin(), arg.name.GetString());
    }
    else {
        err = _kernelHandle->ReadBufferData(&result, arg.name.GetString());
    }

    ctx->Finish();
    if (err != CL_SUCCESS) {
        std::cout << "Failed to read buffer data of parameter: " << arg.name
                  << std::endl;
        return {};
    }
    return result;
}

} // namespace Deformer

PXR_NAMESPACE_CLOSE_SCOPE

#endif