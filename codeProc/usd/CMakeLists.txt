file(GLOB sources "*.cpp")
file(GLOB headers "*.h")
list(FILTER sources EXCLUDE REGEX "./module*|./wrap*")
file(GLOB py_sources "[module|wrap]*.cpp")

find_package(peasyocl REQUIRED)

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
            ${peasyocl_INCLUDE_DIR}
        LIBS
            ${PXR_LIBRARIES}
            peasyocl
    )
endif()


if (${BUILD_HOUDINI_PLUGIN})
    # HOUDINI LIBRARY
    
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    
    set(HPLUGIN_NAME ${USDPLUGIN_NAME}Houdini)
    
    list( APPEND CMAKE_PREFIX_PATH "${HOUDINI_INSTALL_ROOT}/toolkit/cmake" )
    
    find_package(Houdini REQUIRED)

    set(PYTHON_LIB python3.10)
    set(PYTHON_LIB_NUMBER python310)
    set(H_INCLUDE_DIR "${HOUDINI_INSTALL_ROOT}/toolkit/include")

    if (${APPLE})
        set(H_LIB_DIR "${HOUDINI_INSTALL_ROOT}/../Libraries")
        set(PYTHON_LIB_DIR "${HOUDINI_INSTALL_ROOT}/Frameworks/Python.framework/Versions/3.10")
        set(PYTHON_FRAMEWORK "${HOUDINI_INSTALL_ROOT}/Frameworks/Python.framework")
    else()
        set(H_LIB_DIR "${HOUDINI_INSTALL_ROOT}/dsolib")
        set(PYTHON_LIB_DIR "${HOUDINI_INSTALL_ROOT}/python/lib")
        set(PYTHON_FRAMEWORK "")
    endif()

    set(PYTHON_INCLUDE_PATH "${H_INCLUDE_DIR}/${PYTHON_LIB}")
    set(PXR_INCLUDE_DIR ${H_INCLUDE_DIR})

    set(BOOST_NAMESPACE hboost)
    set(BOOST_PYTHON_LIB ${BOOST_NAMESPACE}_${PYTHON_LIB_NUMBER})
    set(BOOST_INCLUDE_DIR ${H_INCLUDE_DIR}/${BOOST_NAMESPACE})

    link_directories(${HPLUGIN_NAME} PUBLIC ${H_LIB_DIR})
    link_directories(${HPLUGIN_NAME} PUBLIC ${PYTHON_LIB_DIR}/lib)

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
            ${peasyocl_INCLUDE_DIR}
        LIBS
            Houdini
            ${PYTHON_FRAMEWORK}
            peasyocl
        )

    endif()