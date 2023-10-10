#ifndef OCL_DEFORMER_CONTEXT_H
#define OCL_DEFORMER_CONTEXT_H

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120

#include <memory>
#include <string>
#include <vector>
#include <array>

// #include <OpenCL/opencl.h>
#include "opencl.hpp"

namespace ocl {

class DeformerContext {
public:
    DeformerContext();
    DeformerContext(const std::string& name);

    int Init(const std::string& kernelName);

    int LoadShader(); // Use _kernelName from Init
    int LoadShader(const std::string_view& name);

    template<typename T>
    int AddArgument(cl_mem_flags flags, const size_t& size, const bool createBuffer, T* data=nullptr);

    template<size_t size>
    int AddIndicesArray(const std::array<int, size>& indices);

    template<size_t size>
    int AddLengthsArray(const std::array<int, size>& lengths);

    template<typename T>
    int SetBufferData(T* data, cl::Buffer buffer, const size_t size);

    template<typename T>
    int SetBufferData(T* data, const int& bufferIndex, const size_t size);

    template<typename T>
    int ReadBufferData(T* data, cl::Buffer buffer, const size_t size);

    template<typename T>
    int ReadBufferData(T* data, const int& bufferIndex, const size_t size);

    int Execute(const size_t& global);

    bool initialized = false;

private:
    std::string  _kernelName;
    std::string  _kernelCode;
    size_t       _kernelCodeLength;

    cl::Context       _context;
    cl::CommandQueue  _queue;
    cl::Program       _program;
    cl::Kernel        _kernel;
    cl::Device        _device;

    std::vector<cl::Buffer> _buffers;

    int _argumentsSize = 0;
};

template<typename T>
inline int DeformerContext::AddArgument(cl_mem_flags flags, const size_t& size, const bool createBuffer, T* data) {
    if (createBuffer) {
        cl::Buffer d_data(_context, flags, sizeof(T) * size);

        if (data != nullptr) {
            SetBufferData(data, d_data, size);
        }

        _kernel.setArg(_argumentsSize, sizeof(cl_mem), &d_data);
        _buffers.push_back(d_data);

    } else {
        _kernel.setArg(_argumentsSize, sizeof(T), data);
    }

    _argumentsSize++;
    return 0;
}


// template<size_t size>
// inline int DeformerContext::AddIndicesArray(const std::array<int, size>& indices) {
//     return AddArgument<int>(CL_MEM_READ_ONLY, size, false, indices.begin());
// }

// template<size_t size>
// inline int DeformerContext::AddLengthsArray(const std::array<int, size>& lengths) {
//     return AddArgument<int>(CL_MEM_READ_ONLY, size, false, lengths.begin());
// }

template<typename T>
inline int DeformerContext::SetBufferData(T* data, cl::Buffer buffer, const size_t size) {
    cl_int err = _queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(T) * size, data);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to write data to source array!\n");
        return 1;
    }
    return 0;
}

template<typename T>
inline int DeformerContext::SetBufferData(T* data, const int& bufferIndex, const size_t size) {
    return SetBufferData(data, _buffers[bufferIndex], size);
}

template<typename T>
inline int DeformerContext::ReadBufferData(T* data, cl::Buffer buffer, const size_t size) {
    cl_int err = _queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(T) * size, data);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to read output array! %d\n", err);
        return 1;
    }
    return 0;
}

template<typename T>
inline int DeformerContext::ReadBufferData(T* data, const int& bufferIndex, const size_t size) {
    return ReadBufferData<T>(data, _buffers[bufferIndex], size);
}


}

#endif