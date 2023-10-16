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

#include "opencl.hpp"

namespace ocl {

struct KernelHandle {
    cl::Kernel kernel;
    std::vector<cl::Buffer> buffers;
    cl::CommandQueue* queue;
    cl::Context* context;

    int argCount = 0;

    template<typename T>
    int AddArgument(cl_mem_flags flags, const size_t& size, const bool createBuffer, T* data=nullptr);
    template<typename It>
    int AddArgument(cl_mem_flags flags, const size_t& size, It start);

    template<typename mem, typename T>
    int SetArgument(const int argIndex, T* data);

    template<typename T>
    int ReadBufferData(T* data, cl::Buffer buffer, const size_t size);
    template<typename It>
    int ReadBufferData(It start, cl::Buffer buffer, const size_t size);

    template<typename T>
    int SetBufferData(T* data, cl::Buffer buffer, const size_t size);
    template<typename It>
    int SetBufferData(It start, cl::Buffer buffer, const size_t size);

    template<typename T>
    int ReadBufferData(T* data, const int& bufferIndex, const size_t size);
    template<typename It>
    int ReadBufferData(It start, const int& bufferIndex, const size_t size);

    template<typename T>
    int SetBufferData(T* data, const int& bufferIndex, const size_t size);
    template<typename It>
    int SetBufferData(It start, const int& bufferIndex, const size_t size);

};

class DeformerContext {
public:
    DeformerContext();

    int Build();

    // Add a .cl or .ocl file for executing
    int AddSource(const std::string& fileName);

    // Load a kernel from the sources added. Returns a KernelHandle which is used to access kernel specific
    // arguments.
    KernelHandle* AddKernel(const std::string& name, int* err=nullptr);
    KernelHandle* GetKernelHandle(const std::string& name);

    // Execute a kernel with name kernelName
    int Execute(const size_t& global, const std::string& kernelName);

    void Finish();
    // Is true once everything is initialized
    bool initialized = false;

private:

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
inline int KernelHandle::AddArgument(cl_mem_flags flags, const size_t& size, const bool createBuffer, T* data) {
    if (createBuffer) {
        cl::Buffer d_data(*context, flags, sizeof(T) * size);

        if (data != nullptr) {
            SetBufferData(data, d_data, size);
        }

        SetArgument<cl_mem, cl::Buffer>(argCount, &d_data);
        buffers.push_back(d_data);

    } else {
        SetArgument<T, T>(argCount, data);
    }
    argCount++;
    return 0;
}

template<typename It>
inline int KernelHandle::AddArgument(cl_mem_flags flags, const size_t& size, It start) {

    typename It::value_type T = *start;
    cl::Buffer d_data(*context, flags, sizeof(T) * size);

    SetBufferData(start, d_data, size);

    SetArgument<cl_mem, cl::Buffer>(argCount, &d_data);
    buffers.push_back(d_data);

    argCount++;
    return 0;
}

template<typename mem, typename T>
inline int KernelHandle::SetArgument(const int argIndex, T* data) {
    kernel.setArg(argIndex, sizeof(mem), data);
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

template<typename It>
inline int KernelHandle::SetBufferData(It start, cl::Buffer buffer, const size_t size) {
    typename It::value_type T = *start;
    cl_int err = queue->enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(T) * size, static_cast<void*>(&start));
    if (err != CL_SUCCESS) {
        printf("Error: Failed to write data to source array!\n");
        return 1;
    }
    return 0;
}

template<typename T>
inline int KernelHandle::SetBufferData(T* data, const int& bufferIndex, const size_t size) {
    return SetBufferData(data, buffers[bufferIndex], size);
}

template<typename It>
inline int KernelHandle::SetBufferData(It start, const int& bufferIndex, const size_t size) {
    return SetBufferData(start, buffers[bufferIndex], start);
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

template<typename It>
inline int KernelHandle::ReadBufferData(It start, cl::Buffer buffer, const size_t size) {
    cl_int err = queue->enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(It::value_type) * size, static_cast<void*>(start));
    if (err != CL_SUCCESS) {
        printf("Error: Failed to read output array! %d\n", err);
        return 1;
    }
    return 0;
}

template<typename T>
inline int KernelHandle::ReadBufferData(T* data, const int& bufferIndex, const size_t size) {
    return ReadBufferData(data, buffers[bufferIndex], size);
}

template<typename It>
inline int KernelHandle::ReadBufferData(It start, const int& bufferIndex, const size_t size) {
    return ReadBufferData(start, buffers[bufferIndex], size);
}



}

#endif