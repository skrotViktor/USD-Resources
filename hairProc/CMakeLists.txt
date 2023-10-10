cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(hairProc VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USDPLUGIN_NAME hairProc)
set(PYPACKAGE_NAME vik)
set(OCLMODULE_NAME ocl)



# set(USDIMAGINGGL_ENGINE_ENABLE_SCENE_INDEX true)

set(USD_INSTALL_ROOT /opt/USD)
set(CMAKE_INSTALL_PREFIX "/opt/USD_resources" CACHE PATH "..." FORCE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_definitions(-D_GLIBCXX_USE_CXX_ABI=0)

add_subdirectory(usd)
add_subdirectory(ocl)

if(${APPLE})
    target_link_options(
        ${USDPLUGIN_NAME}
        PRIVATE -framework OpenCL
    )
endif()

