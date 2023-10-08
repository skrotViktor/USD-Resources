set(OCLMODULE_TARGETS_NAME "${USDPLUGIN_NAME}Targets")

find_package(OpenCL REQUIRED)

add_compile_definitions(OCL_FILE_PATH="${CMAKE_INSTALL_PREFIX}/ocl/kernels")

file(GLOB sources "*.cpp")
file(GLOB headers "*.h")

file(GLOB OCL "kernels/*.cl")

install(
    FILES ${OCL}
    DESTINATION "ocl"
)

# MODULE LIBRARY
add_library(${OCLMODULE_NAME}
    ${headers}
    ${sources}
)

target_link_libraries(${OCLMODULE_NAME}
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
