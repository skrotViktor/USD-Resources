
# SET DEFAULTS


file(GLOB sources "*.cpp")
file(GLOB headers "*.h")
list(FILTER sources EXCLUDE REGEX "./module*|./wrap*")

file(GLOB py_sources "[module|wrap]*.cpp")

if (${BUILD_USD_PLUGIN})
    find_package(Python3 REQUIRED)
    set(CMAKE_FIND_PACKAGE_REDIRECTS_DIR ${USD_INSTALL_ROOT})
    find_package(pxr REQUIRED)

    create_target(
            ${USDPLUGIN_NAME}
        PYPACKAGE_NAME
            ${PYPACKAGE_NAME}
        HEADERS
            ${headers}
        SOURCES
            ${sources}
        PY_SOURCES
            ${py_sources}
        INCLUDE_DIRS
            ${PYTHON_INCLUDE_PATH}
            ${PXR_INCLUDE_DIRS}
            ".."
        LIBS
            ${PXR_LIBRARIES}
            ${OCLMODULE_NAME}
    )
    # get_py_module_name(${USDPLUGIN_NAME} MODULE_NAME)
    # 

    # add_library(${USDPLUGIN_NAME}
    #     SHARED
    #         ${headers}
    #         ${sources}
    # )

    # # install(
    # #     FILES
    # #         ${headers}
    # #     DESTINATION
    # #         include/usd
    # # )

    # target_include_directories(${USDPLUGIN_NAME}
    #     PRIVATE
    #         ${PYTHON_INCLUDE_PATH}
    #         ${PXR_INCLUDE_DIRS}
    #         ".."
    # )

    # target_link_libraries(${USDPLUGIN_NAME}
    #     PUBLIC
    #         ${PXR_LIBRARIES}
    #         ${OCLMODULE_NAME}
    # )

    # set_target_properties(${USDPLUGIN_NAME} PROPERTIES PREFIX "")

    # target_compile_features(${USDPLUGIN_NAME}
    #     PUBLIC
    #         cxx_auto_type
    # )

    # configure_file(
    #     plugInfo.json
    #     ${CMAKE_BINARY_DIR}/plugInfo.json
    #     @ONLY
    # )

    # target_compile_definitions(${USDPLUGIN_NAME}
    #     PRIVATE
    #         MFB_PACKAGE_NAME=${USDPLUGIN_NAME}
    #         MFB_ALT_PACKAGE_NAME=${USDPLUGIN_NAME}
    #         MFB_PACKAGE_MODULE=${MODULE_NAME}
    # )

    # install(
    #     FILES ${CMAKE_BINARY_DIR}/plugInfo.json
    #     DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources"
    # )

    # install(
    #     TARGETS ${USDPLUGIN_NAME}
    #     # EXPORT ${USDPLUGIN_TARGETS_NAME}
    #     LIBRARY DESTINATION lib
    #     INCLUDES DESTINATION include
    # )

    # install(
    #     FILES generatedSchema.usda
    #     DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources"
    # )

    # install(
    #     FILES schema.usda
    #     DESTINATION "lib/usd/${USDPLUGIN_NAME}/resources/${USDPLUGIN_NAME}"
    # )

    # # PYTHON

    # set(USDPLUGIN_PYTHON_NAME _${USDPLUGIN_NAME})

    # add_library(${USDPLUGIN_PYTHON_NAME}
    #     SHARED
    #         ${module_sources}
    #         ${wrap_sources}
    # )

    # set_target_properties(${USDPLUGIN_PYTHON_NAME}
    # PROPERTIES
    #     INSTALL_RPATH "@loader_path/../../.."
    # )

    # target_include_directories(
    #     ${USDPLUGIN_PYTHON_NAME}
    #     PRIVATE
    #         ${PYTHON_INCLUDE_PATH}
    #         ${PXR_INCLUDE_DIRS}
    # )

    # set_target_properties(${USDPLUGIN_PYTHON_NAME} PROPERTIES SUFFIX ".so")

    # set_target_properties(${USDPLUGIN_PYTHON_NAME}
    #     PROPERTIES
    #         PREFIX ""
    # )

    # target_compile_definitions(${USDPLUGIN_PYTHON_NAME}
    #     PRIVATE
    #         MFB_PACKAGE_NAME=${USDPLUGIN_NAME}
    #         MFB_ALT_PACKAGE_NAME=${USDPLUGIN_NAME}
    #         MFB_PACKAGE_MODULE=${MODULE_NAME}
    # )

    # target_link_libraries(${USDPLUGIN_PYTHON_NAME}
    #     ${USDPLUGIN_NAME}
    # )

    # set(PYTHON_PACKAGE_RELATIVE_PATH lib/python/${PYPACKAGE_NAME})
    # set(INSTALL_PYTHONPACKAGE_DIR ${PYTHON_PACKAGE_RELATIVE_PATH})
    # set(INSTALL_WRAPPER_DIR ${INSTALL_PYTHONPACKAGE_DIR}/${MODULE_NAME})

    # install(
    #     FILES __init__.py
    #     DESTINATION ${INSTALL_WRAPPER_DIR}
    # )

    # install(
    #     TARGETS ${USDPLUGIN_PYTHON_NAME}
    #     DESTINATION ${INSTALL_WRAPPER_DIR}
    # )

    # install(
    #     FILES __packageinit__.py
    #     DESTINATION ${INSTALL_PYTHONPACKAGE_DIR}
    #     RENAME __init__.py
    # )
endif()


if (${BUILD_HOUDINI_PLUGIN})
    # HOUDINI LIBRARY
    
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    # set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    
    set(HPLUGIN_NAME ${USDPLUGIN_NAME}Houdini)

    set(HOUDINI_INSTALL_ROOT "/Applications/Houdini/current/Frameworks/Houdini.framework/Resources")
    
    list( APPEND CMAKE_PREFIX_PATH "${HOUDINI_INSTALL_ROOT}/toolkit/cmake" )
    find_package(Houdini REQUIRED)
    
    set(H_LIB_DIR "${HOUDINI_INSTALL_ROOT}/../Libraries")
    set(H_INCLUDE_DIR "${HOUDINI_INSTALL_ROOT}/toolkit/include")
    
    set(PXR_INCLUDE_DIR ${H_INCLUDE_DIR})
    
    set(PYTHON_LIB python3.10)
    set(PYTHON_LIB_NUMBER python310)
    
    set(PYTHON_LIB_DIR "${HOUDINI_INSTALL_ROOT}/Frameworks/Python.framework/Versions/3.10/lib")
    set(PYTHON_LIB_SITEPACKAGES "${PYTHON_LIB_DIR}/${PYTHON_LIB}/site-packages")
    set(PYTHON_INCLUDE_PATH "${H_INCLUDE_DIR}/${PYTHON_LIB}")
    
    set(BOOST_NAMESPACE hboost)
    set(BOOST_PYTHON_LIB ${BOOST_NAMESPACE}_${PYTHON_LIB_NUMBER})
    set(BOOST_INCLUDE_DIR ${H_INCLUDE_DIR}/${BOOST_NAMESPACE})
    
    include_directories(${CMAKE_SOURCE_DIR}/usd)
    link_directories(${H_LIB_DIR})
    link_directories(${PYTHON_LIB_DIR})

    create_target(
        ${HPLUGIN_NAME}
        BUILD_HOUDINI
        PYPACKAGE_NAME
            ${PYPACKAGE_NAME}
        HEADERS
            ${headers}
        SOURCES
            ${sources}
        PY_SOURCES
            ${py_sources}
        INCLUDE_DIRS
            ${PYTHON_INCLUDE_PATH}
            ${PXR_INCLUDE_DIR}
            ".."
        LIBS
            Houdini
            /Applications/Houdini/Houdini20.0.506/Frameworks/Python.framework
            ${OCLMODULE_NAME}
    )

    
    # add_library(${HPLUGIN_NAME}
    #     SHARED
    #         ${headers}
    #         ${sources}
    # )

    # install(
    #     FILES
    #         ${headers}
    #     DESTINATION
    #         include/usd
    # )

    # target_include_directories(${HPLUGIN_NAME}
    #     PUBLIC
    #         ${PYTHON_INCLUDE_PATH}
    #         ${PXR_INCLUDE_DIR}
    #         ".."
    # )
    
    # target_link_libraries(${HPLUGIN_NAME}
    #     PUBLIC
    #         Houdini
    #         "/Applications/Houdini/Houdini20.0.506/Frameworks/Python.framework"
    #         ${OCLMODULE_NAME}
    # )

    # set_target_properties(${HPLUGIN_NAME} PROPERTIES PREFIX "")

    # target_compile_features(${HPLUGIN_NAME}
    #     PUBLIC
    #         cxx_auto_type
    # )

    # configure_file(
    #     plugInfo.json
    #     ${CMAKE_BINARY_DIR}/plugInfo.json
    #     @ONLY
    # )

    # target_compile_definitions(${HPLUGIN_NAME}
    #     PRIVATE
    #         MFB_PACKAGE_NAME=${HPLUGIN_NAME}
    #         MFB_ALT_PACKAGE_NAME=${HPLUGIN_NAME}
    #         MFB_PACKAGE_MODULE=${MODULE_NAME}
    #     PUBLIC
    #         BUILD_HOUDINI_PLUGIN=1
    # )

    # install(
    #     FILES ${CMAKE_BINARY_DIR}/plugInfo.json
    #     DESTINATION "lib/usd/${HPLUGIN_NAME}/resources"
    # )

    # install(
    #     TARGETS ${HPLUGIN_NAME}
    #     # EXPORT ${USDPLUGIN_TARGETS_NAME}
    #     LIBRARY DESTINATION lib
    #     INCLUDES DESTINATION include
    # )

    # # CMAKE CONFIG
    # install(
    #     FILES generatedSchema.usda
    #     DESTINATION "lib/usd/${HPLUGIN_NAME}/resources"
    # )

    # install(
    #     FILES schema.usda
    #     DESTINATION "lib/usd/${HPLUGIN_NAME}/resources/${HPLUGIN_NAME}"
    # )

    # # PYTHON

    # set(USDPLUGIN_PYTHON_NAME _${HPLUGIN_NAME})

    # add_library(${USDPLUGIN_PYTHON_NAME}
    #     SHARED
    #         ${module_sources}
    #         ${wrap_sources}
    # )

    # set_target_properties(${USDPLUGIN_PYTHON_NAME}
    #     PROPERTIES
    #         INSTALL_RPATH "@loader_path/../../.."
    # )

    # target_include_directories(
    #     ${USDPLUGIN_PYTHON_NAME}
    #     PRIVATE
    #         ${PYTHON_INCLUDE_PATH}
    #         ${PXR_INCLUDE_DIR}
    # )

    # set_target_properties(${USDPLUGIN_PYTHON_NAME} PROPERTIES SUFFIX ".so")

    # set_target_properties(${USDPLUGIN_PYTHON_NAME}
    #     PROPERTIES
    #         PREFIX ""
    # )

    # target_compile_definitions(${USDPLUGIN_PYTHON_NAME}
    #     PRIVATE
    #         MFB_PACKAGE_NAME=${HPLUGIN_NAME}
    #         MFB_ALT_PACKAGE_NAME=${HPLUGIN_NAME}
    #         MFB_PACKAGE_MODULE=${MODULE_NAME}
    # )

    # target_link_libraries(${USDPLUGIN_PYTHON_NAME}
    #     ${HPLUGIN_NAME}
    #     ${BOOST_PYTHON_LIB}
    # )

    # set(PYTHON_PACKAGE_RELATIVE_PATH lib/python/${PYPACKAGE_NAME})
    # set(INSTALL_PYTHONPACKAGE_DIR ${PYTHON_PACKAGE_RELATIVE_PATH})
    # set(INSTALL_WRAPPER_DIR ${INSTALL_PYTHONPACKAGE_DIR}/${MODULE_NAME})

    # install(
    #     FILES __init__.py
    #     DESTINATION ${INSTALL_WRAPPER_DIR}
    # )

    # install(
    #     TARGETS ${USDPLUGIN_PYTHON_NAME}
    #     DESTINATION ${INSTALL_WRAPPER_DIR}
    # )

    # install(
    #     FILES __packageinit__.py
    #     DESTINATION ${INSTALL_PYTHONPACKAGE_DIR}
    #     RENAME __init__.py
    # )
endif()