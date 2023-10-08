#include "DeformerContext.h"
#include <iostream>
#include <vector>
#include <fstream>

#include "KernelUtils.h"

namespace ocl {

DeformerContext::DeformerContext(const std::string& name) : _kernelName(name) {
    int err = LoadShader(_kernelName);
    if (err) {
        printf("Error: Failed to load kernel source!\n");
        initialized = false;
        return;
    }

    int gpu = 1;
    err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &_device, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        initialized = false;
        return;
    }

    _context = clCreateContext(0, 1, &_device, NULL, NULL, &err);
    if (!_context || err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        initialized = false;
        return;
    }

    _commands = clCreateCommandQueue(_context, _device, 0, &err);
    if (!_commands || err != CL_SUCCESS) {
        printf("Error: Failed to create a command commands!\n");
        initialized = false;
        return;
    }

    const char* cstr = _kernelCode.c_str();
    _program = clCreateProgramWithSource(_context, 1, (const char **) & cstr, NULL, &err);
    if (!_program || err != CL_SUCCESS) {
        printf("Error: Failed to create compute program!\n");
        initialized = false;
        return;
    }

    err = clBuildProgram(_program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
 
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        initialized = false;
        return;
    }

    _kernel = clCreateKernel(_program, _kernelName.c_str(), &err);
    if (!_kernel || err != CL_SUCCESS) {
        printf("Error: Failed to create compute kernel!\n");
        initialized = false;
        return;
    }
    initialized = true;
}

DeformerContext::~DeformerContext() {
    clReleaseProgram(_program);
    clReleaseKernel(_kernel);
    clReleaseCommandQueue(_commands);
    clReleaseContext(_context);
    for(auto& it: _buffers) {
        clReleaseMemObject(it);
    }
}

int DeformerContext::LoadShader() {
    return LoadShader(_kernelName);
}

int DeformerContext::LoadShader(const std::string_view& name) {
    std::string kernelPath = utils::GetKernelByName(name.data());
    if (kernelPath.empty()) {
        return 1;
    }

    std::string kernelSource = utils::LoadFile(kernelPath);
    if (kernelSource.empty()) {
        return 1;
    }

    _kernelCode = kernelSource;
    return 0;
}

} // close namespace