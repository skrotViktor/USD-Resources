set(OCLMODULE_TARGETS_NAME "${USDPLUGIN_NAME}Targets")

find_package(OpenCL REQUIRED)

add_compile_definitions(OCL_FILE_PATH="${CMAKE_INSTALL_PREFIX}/ocl/kernels")

set(OPENCL_CLHPP_HEADERS_DIR .)

file(GLOB sources "*.cpp")
file(GLOB headers "*.h")

file(GLOB OCL "kernels/*.cl")

install(
    FILES ${OCL}
    DESTINATION "ocl"
)

# OCL LIBRARY
add_library(${OCLMODULE_NAME}
    SHARED
        ${headers}
        ${sources}
)

target_link_libraries(${OCLMODULE_NAME}
    PUBLIC
        ${OpenCL_LIBRARIES}
)

install(
    FILES
        ${headers}
    DESTINATION
        include/ocl
)

install(
    TARGETS ${OCLMODULE_NAME}
    EXPORT ${OCLMODULE_TARGETS_NAME}
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)
