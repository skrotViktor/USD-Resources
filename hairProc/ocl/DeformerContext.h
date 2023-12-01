#ifndef OCL_DEFORMER_CONTEXT_H
#define OCL_DEFORMER_CONTEXT_H

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

// #define CL_HPP_CL_1_2_DEFAULT_BUILD "-cl-std=CL1.2"

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>

#include <iostream>

#include "opencl.hpp"

namespace ocl {

struct KernelHandle {
    cl::Kernel kernel;
    std::unordered_map<std::string, cl::Buffer> arguments;
    cl::CommandQueue* queue;
    cl::Context* context;

    int argCount = 0;

    template<typename T>
    int AddArgument(cl_mem_flags flags, const std::string& name, const size_t& size, const bool createBuffer, T* data=nullptr);
    template<typename T>
    int AddArgument(cl_mem_flags flags, const std::string& name, const size_t& size, const bool createBuffer, const T& data);

    template<typename mem, typename T>
    int SetArgument(const int argIndex, T* data);

    template<typename T>
    int SetArgument(const int argIndex, const T& data);

    template<typename mem, typename T>
    int SetArgument(const std::string& name, T* data);

    template<typename T>
    int SetArgument(const std::string& name, const T& data);

    template<typename T>
    int ReadBufferData(T* data, cl::Buffer buffer, const size_t size);

    template<typename T>
    int SetBufferData(T* data, cl::Buffer buffer, const size_t size);

    template<typename T>
    int ReadBufferData(T* data, const std::string& bufferName, const size_t size);

    template<typename T>
    int SetBufferData(T* data, const std::string& bufferName, const size_t size);
};


// Singleton class for handling opencl
class DeformerContext {
public:
    int Init();
    int Build();

    static DeformerContext* getInstance() {
        static DeformerContext* ctx = new DeformerContext();
        return ctx;
    }

    // Add a .cl or .ocl file for executing
    int AddSource(const std::string& fileName);

    // Load a kernel from the sources added. Returns a KernelHandle which is used to access kernel specific
    // arguments.
    KernelHandle* AddKernel(const std::string& kernelName, const std::string& key="", int* err=nullptr);
    KernelHandle* GetKernelHandle(const std::string& name);

    bool HasKernel(const std::string& name) {return _kernels.find(name) != _kernels.end();}

    // Execute a kernel with name kernelName
    int Execute(const size_t& global, const std::string& kernelName);

    void Finish();
    // Is true once everything is initialized
    bool initialized = false;
    bool built = false;

private:
    DeformerContext() = default;
    DeformerContext(const DeformerContext&) = delete;
    DeformerContext(DeformerContext&&) = delete;
    DeformerContext& operator = (const DeformerContext&) = delete;
    DeformerContext& operator = (DeformerContext&&) = delete;

    cl::string _LoadShader(const std::string_view& fileName, int* err);

    cl::vector<cl::string>  _kernelNames;
    cl::vector<cl::string>  _kernelCodes;

    cl::Context                         _context;
    cl::CommandQueue                    _queue;
    cl::Program                         _program;
    std::map<cl::string, KernelHandle>  _kernels;
    cl::Device                          _device;

    int _argumentsSize = 0;
};

template<typename T>
inline int KernelHandle::AddArgument(cl_mem_flags flags, const std::string& name, const size_t& size, const bool createBuffer, T* data) {
    if (createBuffer) {
        cl::Buffer d_data(*context, flags, sizeof(T) * size);

        if (data != nullptr) {
            SetBufferData(data, d_data, size);
        }

        SetArgument<cl_mem, cl::Buffer>(argCount, &d_data);
        arguments.insert({name, d_data});
    } else {
        SetArgument<T, T>(argCount, data);
    }
    argCount++;
    return 0;
}

template<typename T>
inline int KernelHandle::AddArgument(cl_mem_flags flags, const std::string& name, const size_t& size, const bool createBuffer, const T& data) {
    if (createBuffer) {
        cl::Buffer d_data(*context, flags, sizeof(T) * size);

        SetBufferData(&data, d_data, size);        

        SetArgument<cl_mem, cl::Buffer>(argCount, &d_data);
        arguments.insert({name, d_data});
    } else {
        SetArgument<T>(argCount, data);
    }
    argCount++;
    return 0;
}

template<typename mem, typename T>
inline int KernelHandle::SetArgument(const int argIndex, T* data) {
    kernel.setArg(argIndex, sizeof(mem), data);
    return 0;
}

template<typename T>
inline int KernelHandle::SetArgument(const int argIndex, const T& data) {
    kernel.setArg<T>(argIndex, data);
    return 0;
}

template<typename mem, typename T>
inline int KernelHandle::SetArgument(const std::string& name, T* data) {
    kernel.setArg<T>(arguments[name], data);
    return 0;
}

template<typename T>
inline int KernelHandle::SetArgument(const std::string& name, const T& data) {
    kernel.setArg<T>(arguments[name], data);
    return 0;
}

template<typename T>
inline int KernelHandle::SetBufferData(T* data, cl::Buffer buffer, const size_t size) {
    cl_int err = queue->enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(T) * size, data);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to write data to source array!\n");
        return 1;
    }
    return 0;
}

template<typename T>
inline int KernelHandle::SetBufferData(T* data, const std::string& bufferName, const size_t size) {
    if (arguments.find(bufferName) == arguments.end()) {
        printf("Error: Buffer %s is not recognized!\n", bufferName.c_str());
        return 1;
    }
    return SetBufferData(data, arguments[bufferName], size);
}

template<typename T>
inline int KernelHandle::ReadBufferData(T* data, cl::Buffer buffer, const size_t size) {
    cl_int err = queue->enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(T) * size, data);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to read output array! %d\n", err);
        return 1;
    }
    return 0;
}

template<typename T>
inline int KernelHandle::ReadBufferData(T* data, const std::string& bufferName, const size_t size) {
    return ReadBufferData(data, arguments[bufferName], size);
}

}

#endif