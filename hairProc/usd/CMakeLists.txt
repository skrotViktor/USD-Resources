
# SET DEFAULTS
set(USDPLUGIN_TARGETS_NAME "${USDPLUGIN_NAME}Targets")
set(INSTALL_CONFIGURATION_DIRECTORY "lib/cmake/${USDPLUGIN_NAME}Schemas")

set(PLUG_INFO_LIBRARY_PATH "../../${USDPLUGIN_NAME}.dylib")
set(PLUG_INFO_RESOURCE_PATH "resources")
set(PLUG_INFO_ROOT "..")

set(CONFIGURATION_SOURCE ${CMAKE_CURRENT_LIST_DIR}/cmake/config.cmake)
set(CONFIGURATION_TARGET ${CMAKE_CURRENT_BINARY_DIR}/${USDPLUGIN_NAME}Config.cmake)

find_package(PythonLibs 3.7 REQUIRED)

file(GLOB sources "*.cpp")
file(GLOB headers "*.h")
list(FILTER sources EXCLUDE REGEX "./module*|./wrap*")

file(GLOB module_sources "module*.cpp")
file(GLOB wrap_sources "wrap*.cpp")

# PY MODULE NAME
string(SUBSTRING ${USDPLUGIN_NAME} 0 1 LIBNAME_FL)
string(SUBSTRING ${USDPLUGIN_NAME} 0 1 LIBNAME_FL)
string(TOUPPER ${LIBNAME_FL} LIBNAME_FL)
string(SUBSTRING ${USDPLUGIN_NAME} 1 -1 LIBNAME_SUFFIX)
set(MODULE_NAME
    "${LIBNAME_FL}${LIBNAME_SUFFIX}"
)

# MODULE LIBRARY
add_library(${USDPLUGIN_NAME}
    SHARED
        ${headers}
        ${sources}
)

install(
    FILES
        ${headers}
    DESTINATION
        include/usd
)

set(CMAKE_FIND_PACKAGE_REDIRECTS_DIR ${USD_INSTALL_ROOT})
find_package(pxr REQUIRED)

target_include_directories(${USDPLUGIN_NAME}
    PRIVATE
        ${PYTHON_INCLUDE_PATH}
        ${PXR_INCLUDE_DIRS}
        ".."
)

target_link_libraries(${USDPLUGIN_NAME}
    PUBLIC
        ${PXR_LIBRARIES}
        ${OCLMODULE_NAME}
)

set_target_properties(${USDPLUGIN_NAME} PROPERTIES PREFIX "")

target_compile_features(${USDPLUGIN_NAME}
    PUBLIC
        cxx_auto_type
)

configure_file(
    plugInfo.json
    ${CMAKE_BINARY_DIR}/plugInfo.json
    @ONLY
)

target_compile_definitions(${USDPLUGIN_NAME}
    PRIVATE
        MFB_PACKAGE_NAME=${USDPLUGIN_NAME}
        MFB_ALT_PACKAGE_NAME=${USDPLUGIN_NAME}
        MFB_PACKAGE_MODULE=${MODULE_NAME}
)

install(
    FILES ${CMAKE_BINARY_DIR}/plugInfo.json
    DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources"
)

install(
    TARGETS ${USDPLUGIN_NAME}
    EXPORT ${USDPLUGIN_TARGETS_NAME}
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

install(
    EXPORT ${USDPLUGIN_TARGETS_NAME}
    FILE ${USDPLUGIN_TARGETS_NAME}.cmake
    NAMESPACE ${USDPLUGIN_NAME}::
    DESTINATION ${INSTALL_CONFIGURATION_DIRECTORY}
)

# CMAKE CONFIG
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CONFIGURATION_SOURCE}
    ${CONFIGURATION_TARGET}
    INSTALL_DESTINATION ${INSTALL_CONFIGURATION_DIRECTORY}
)

install(FILES
    ${CONFIGURATION_TARGET}
    DESTINATION ${INSTALL_CONFIGURATION_DIRECTORY}
)

install(
    FILES generatedSchema.usda
    DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources"
)

install(
    FILES schema.usda
    DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources/${USDPLUGIN_NAME}"
)

# PYTHON

set(USDPLUGIN_PYTHON_NAME _${USDPLUGIN_NAME})

add_library(${USDPLUGIN_PYTHON_NAME}
    SHARED
        ${module_sources}
        ${wrap_sources}
)

set_target_properties(${USDPLUGIN_PYTHON_NAME}
  PROPERTIES
    INSTALL_RPATH "@loader_path/../../.."
)

target_include_directories(
    ${USDPLUGIN_PYTHON_NAME}
    PRIVATE
        ${PYTHON_INCLUDE_PATH}
        ${PXR_INCLUDE_DIRS}
)

set_target_properties(${USDPLUGIN_PYTHON_NAME} PROPERTIES SUFFIX ".so")

set_target_properties(${USDPLUGIN_PYTHON_NAME}
    PROPERTIES
        PREFIX ""
)

target_compile_definitions(${USDPLUGIN_PYTHON_NAME}
    PRIVATE
        MFB_PACKAGE_NAME=${USDPLUGIN_NAME}
        MFB_ALT_PACKAGE_NAME=${USDPLUGIN_NAME}
        MFB_PACKAGE_MODULE=${MODULE_NAME}
)

target_link_libraries(${USDPLUGIN_PYTHON_NAME}
    ${USDPLUGIN_NAME}
)

set(PYTHON_PACKAGE_RELATIVE_PATH lib/python/${PYPACKAGE_NAME})
set(INSTALL_PYTHONPACKAGE_DIR ${PYTHON_PACKAGE_RELATIVE_PATH})
set(INSTALL_WRAPPER_DIR ${INSTALL_PYTHONPACKAGE_DIR}/${MODULE_NAME})

install(
    FILES __init__.py
    DESTINATION ${INSTALL_WRAPPER_DIR}
)

install(
    TARGETS ${USDPLUGIN_PYTHON_NAME}
    DESTINATION ${INSTALL_WRAPPER_DIR}
)

install(
    FILES __packageinit__.py
    DESTINATION ${INSTALL_PYTHONPACKAGE_DIR}
    RENAME __init__.py
)
