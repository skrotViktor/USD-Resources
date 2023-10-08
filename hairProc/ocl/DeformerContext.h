#ifndef OCL_DEFORMER_CONTEXT_H
#define OCL_DEFORMER_CONTEXT_H

// #define CL_HPP_TARGET_OPENCL_VERSION 120
// #define CL_HPP_MINIMUM_OPENCL_VERSION 120

#include <memory>
#include <string>
#include <vector>
#include <array>

#include <OpenCL/opencl.h>

namespace ocl {

// template<typename T>
// struct OclParameter {
//     cl_mem_flags flags;
// };

class DeformerContext {
public:
    DeformerContext(const std::string& name);
    ~DeformerContext();

    int LoadShader(); // Use _kernelName from Init
    int LoadShader(const std::string_view& name);

    template<typename T>
    int AddParameter(cl_mem_flags flags, const size_t size, T* data=nullptr);

    template<typename T>
    int SetBufferData(T* data, cl_mem buffer, const size_t size);

    template<typename T>
    int SetBufferData(T* data, int bufferIndex, const size_t size);

    template<typename T, size_t size>
    std::array<T, size> Execute();

    bool initialized = false;

private:
    std::string  _kernelName;
    std::string  _kernelCode;
    size_t       _kernelCodeLength;

    cl_context          _context;
    cl_command_queue    _commands;
    cl_program          _program;
    cl_kernel           _kernel;
    cl_device_id        _device;

    std::vector<cl_mem> _buffers;
};

// Template declarations
template<typename T>
inline int DeformerContext::AddParameter(cl_mem_flags flags, const size_t size, T* data) {
    cl_mem buffer = clCreateBuffer(_context, flags, sizeof(T) * size, NULL, NULL);
    if (!buffer) {
        printf("Error: Failed to create input buffer\n");
        return 1;
    }

    if (data != nullptr) {
        int err = SetBufferData(data, buffer, size);
        if (err != CL_SUCCESS) {
            return 1;
        }
    }

    int err = clSetKernelArg(_kernel, _buffers.size(), sizeof(cl_mem), &buffer);
    if (err != CL_SUCCESS) {
        return 1;
    }
    _buffers.push_back(buffer);
    return 0;
}

template<typename T>
inline int DeformerContext::SetBufferData(T* data, cl_mem buffer, const size_t size) {
    int err = clEnqueueWriteBuffer(_commands, buffer, CL_TRUE, 0, sizeof(T) * size, data, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to write data to source array!\n");
        return 1;
    }
    return 0;
}

template<typename T>
inline int DeformerContext::SetBufferData(T* data, int bufferIndex, const size_t size) {
    return SetBufferData(data, _buffers[bufferIndex], size);
}

template<typename T, size_t size>
std::array<T, size> DeformerContext::Execute() {
    size_t local;
    std::array<T, size> results;

    int err = clGetKernelWorkGroupInfo(_kernel, _device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        initialized = false;
        return results;
    }

    size_t global = size;
    err = clEnqueueNDRangeKernel(_commands, _kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to execute kernel!\n");
        initialized = false;
        return results;
    }
    
    clFinish(_commands);

    err = clEnqueueReadBuffer(_commands, _buffers[1], CL_TRUE, 0, sizeof(T) * size, &results, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to read output array! %d\n", err);
        initialized = false;
        return results;
    }

    return results;
}


}

#endif