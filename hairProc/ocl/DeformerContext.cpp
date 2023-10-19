#include "DeformerContext.h"
#include <iostream>
#include <vector>
#include <fstream>

#include "KernelUtils.h"

namespace ocl {

int DeformerContext::Init() {
    if (initialized) {
        return 1;
    }
    cl_int err;
    _device = cl::Device::getDefault(&err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        return 1;
    }

    _context = cl::Context(_device, nullptr, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a device group!\n");
        return 1;
    }

    _queue = cl::CommandQueue(_context, _device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create a command commands!\n");
        return 1;
    }

    initialized = true;
    return 0;
}

int DeformerContext::AddSource(const std::string& fileName) {
    cl_int err;
    cl::string source = _LoadShader(fileName, &err);
    if (err) {
        printf("Error: Failed to load kernel source!\n");
        initialized = false;
        return 1;
    }
    _kernelCodes.push_back(source);
    return 0;
}

int DeformerContext::Build() {
    if (built) {
        return 0;
    }
    cl_int err;
    _program = cl::Program(_context, _kernelCodes, &err);
    if (err != CL_SUCCESS) {
        built = false;
        printf("Error: Failed to create compute program!\n");
        return 1;
    }

    err = _program.build(_device, 0, 0);
    if (err != CL_SUCCESS) {
        built = false;
        printf("Error: Failed to build program executable!\n");
        return 1;
    }
    built = true;
    return 0;
}

KernelHandle* DeformerContext::AddKernel(const std::string& kernelName, const std::string& key, int* err) {
    cl::Kernel kernel(_program, kernelName.c_str(), err);
    if (err != CL_SUCCESS) {
        initialized = false;
        printf("Error: Failed to create compute kernel with name %s\n", kernelName.c_str());
        return nullptr;
    }
    KernelHandle handle;
    handle.kernel = kernel;
    handle.queue = &_queue;
    handle.context = &_context;

    std::string name = kernelName;
    if (!key.empty()) {
        name = key;
    }
    _kernels[name] = handle;
    return &_kernels[name];
}

KernelHandle* DeformerContext::GetKernelHandle(const std::string& name) {
    return &_kernels[name];
}


int DeformerContext::Execute(const size_t& global, const std::string& kernelName) {
    if (!initialized) {
        return 1;
    }
    if (_kernels.find(kernelName) == _kernels.end()) {
        std::cerr << "Could not find kernel " << kernelName << std::endl;
        return 1;
    }

    size_t local;
    cl_int err;
    err = _kernels[kernelName].kernel.getWorkGroupInfo<size_t>(_device, CL_KERNEL_WORK_GROUP_SIZE, &local);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        return 1;
    }

    cl::Event ev;
    err = _queue.enqueueNDRangeKernel(_kernels[kernelName].kernel, cl::NullRange, cl::NDRange(global), cl::NDRange(1), NULL, &ev);
    ev.wait();
    if (err != CL_SUCCESS) {
        std::cout << "Global: " << global << " Local: " << local << std::endl;
        printf("Error: Failed to execute kernel!\n");
        return 1;
    }

    return 0;
}

void DeformerContext::Finish() {
    _queue.finish();
    _queue.flush();
}

cl::string DeformerContext::_LoadShader(const std::string_view& fileName, int* err) {
    cl::string kernelPath = utils::GetClFileByName(fileName.data());
    if (kernelPath.empty()) {
        *err = 1;
        return "";
    }

    cl::string kernelSource = utils::LoadClKernelSource(kernelPath);
    if (kernelSource.empty()) {
        *err = 1;
        return "";
    }
    *err = 0;

    return kernelSource;
}

} // close namespace