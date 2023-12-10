
# PY MODULE NAME
function(get_py_module_name MODULE_NAME OUT)
    string(SUBSTRING ${MODULE_NAME} 0 1 LIBNAME_FL)
    string(SUBSTRING ${MODULE_NAME} 0 1 LIBNAME_FL)
    string(TOUPPER ${LIBNAME_FL} LIBNAME_FL)
    string(SUBSTRING ${MODULE_NAME} 1 -1 LIBNAME_SUFFIX)
    set(${OUT} "${LIBNAME_FL}${LIBNAME_SUFFIX}" PARENT_SCOPE)
endfunction()


function(create_target TARGET)
    set(PLUG_INFO_LIBRARY_PATH "../../${TARGET}.dylib")
    set(PLUG_INFO_RESOURCE_PATH "resources")
    set(PLUG_INFO_ROOT "..")

    set(oneValueArgs
        PYPACKAGE_NAME
    )
    set(multiValueArgs
        HEADERS
        SOURCES
        PY_SOURCES
        INCLUDE_DIRS
        LIBS
    )
    set(options
        BUILD_HOUDINI
    )

    cmake_parse_arguments(
        args
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    

    get_py_module_name(${TARGET} MODULE_NAME)

    add_library(${TARGET}
        SHARED
            ${args_HEADERS}
            ${args_SOURCES}
    )

    if (${args_BUILD_HOUDINI})
        target_compile_definitions(${TARGET}
            PUBLIC
            BUILD_HOUDINI_PLUGIN
        )
    endif()

    if (args_INCLUDE_DIRS)
        target_include_directories(${TARGET}
            PRIVATE
                ${args_INCLUDE_DIRS}
        )
    endif()

    target_link_libraries(${TARGET}
        PUBLIC
            ${args_LIBS}
    )

    set_target_properties(${TARGET} PROPERTIES PREFIX "")

    target_compile_features(${TARGET}
        PUBLIC
            cxx_auto_type
    )
    
    set(_plugInfo plugInfo.json)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_plugInfo}")

        if(${args_BUILD_HOUDINI})
            set(_plugInfo plugInfoHoudini.json)
        endif()

        configure_file(
            ${_plugInfo}
            ${CMAKE_BINARY_DIR}/${_plugInfo}
            @ONLY
        )
        install(
            FILES ${CMAKE_BINARY_DIR}/${_plugInfo}
            RENAME plugInfo.json
            DESTINATION "lib/usd/${TARGET}/resources"
        )
    endif()

    target_compile_definitions(${TARGET}
        PRIVATE
            MFB_PACKAGE_NAME=${TARGET}
            MFB_ALT_PACKAGE_NAME=${TARGET}
            MFB_PACKAGE_MODULE=${MODULE_NAME}
    )

    install(
        FILES
            ${args_HEADERS}
        DESTINATION
            include/usd
    )

    install(
        TARGETS ${TARGET}
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
    )
    
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/generatedSchema.usda")
        install(
            FILES generatedSchema.usda
            DESTINATION "lib/usd/${TARGET}/resources"
        )
    endif()
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/schema.usda")
        install(
            FILES schema.usda
            DESTINATION "lib/usd/${TARGET}/resources/${TARGET}"
        )
    endif()

    # PYTHON
    # message("${PY_SOURCES}")
    if (args_PY_SOURCES)
        set(USDPLUGIN_PYTHON_NAME _${TARGET})

        add_library(${USDPLUGIN_PYTHON_NAME}
            SHARED
                ${args_PY_SOURCES}
        )

        set_target_properties(${USDPLUGIN_PYTHON_NAME}
        PROPERTIES
            INSTALL_RPATH "@loader_path/../../.."
        )
        if (${args_BUILD_HOUDINI})
            target_compile_definitions(${USDPLUGIN_PYTHON_NAME}
                PUBLIC
                    BUILD_HOUDINI_PLUGIN
            )
        endif()

        target_include_directories(
            ${USDPLUGIN_PYTHON_NAME}
            PUBLIC
                ${INCLUDE_DIRS}
        )

        set_target_properties(${USDPLUGIN_PYTHON_NAME} PROPERTIES SUFFIX ".so")

        set_target_properties(${USDPLUGIN_PYTHON_NAME}
            PROPERTIES
                PREFIX ""
        )

        target_compile_definitions(${USDPLUGIN_PYTHON_NAME}
            PRIVATE
                MFB_PACKAGE_NAME=${TARGET}
                MFB_ALT_PACKAGE_NAME=${TARGET}
                MFB_PACKAGE_MODULE=${MODULE_NAME}
        )

        target_link_libraries(${USDPLUGIN_PYTHON_NAME}
            ${TARGET}
        )

        set(PYTHON_PACKAGE_RELATIVE_PATH lib/python/${args_PYPACKAGE_NAME})
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
    endif()

endfunction()