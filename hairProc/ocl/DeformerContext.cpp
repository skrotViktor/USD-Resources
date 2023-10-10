#include "DeformerContext.h"
#include <iostream>
#include <vector>
#include <fstream>

#include "KernelUtils.h"

namespace ocl {

DeformerContext::DeformerContext() {}

DeformerContext::DeformerContext(const std::string& name) : _kernelName(name) {
    Init(_kernelName);
}

int DeformerContext::Init(const std::string& name) {
    cl_int err = LoadShader(name);
    if (err) {
        printf("Error: Failed to load kernel source!\n");
        initialized = false;
        return 1;
    }

    _device = cl::Device::getDefault(&err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        initialized = false;
        return 1;
    }

    _context = cl::Context(_device, NULL, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        initialized = false;
        return 1;
    }

    _queue = cl::CommandQueue(_context, _device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a command commands!\n");
        initialized = false;
        return 1;
    }

    const char* cstr = _kernelCode.c_str();
    _program = cl::Program(_context, cstr, false, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create compute program!\n");
        initialized = false;
        return 1;
    }

    err = _program.build(_device, 0, 0);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        _program.getBuildInfo<char>(_device, CL_PROGRAM_BUILD_LOG, buffer);
        printf("%s\n", buffer);
        initialized = false;
        return 1;
    }

    _kernel = cl::Kernel(_program, name.c_str(), &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create compute kernel!\n");
        initialized = false;
        return 1;
    }
    initialized = true;
    return 0;
}

int DeformerContext::Execute(const size_t& global) {
    if (!initialized) {
        return 1;
    }
    size_t local;
    cl_int err;
    err = _kernel.getWorkGroupInfo<size_t>(_device, CL_KERNEL_WORK_GROUP_SIZE, &local);

    if (err != CL_SUCCESS) {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        return 1;
    }

    err = _queue.enqueueNDRangeKernel(_kernel, 0, cl::NDRange(global), cl::NDRange(10));
    if (err != CL_SUCCESS) {
        std::cout << "Global: " << global << " Local: " << local << std::endl;
        printf("Error: Failed to execute kernel!\n");
        return 1;
    }
    _queue.finish();
    return 0;
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