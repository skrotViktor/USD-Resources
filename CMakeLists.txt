cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(USD-resources VERSION 0.1.0 LANGUAGES CXX)
set(PYPACKAGE_NAME vik) # The package name used for python. "from vik import HairProc"

# Some RPATH stuff. Don't think this is how you do it....
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Set some compile definitions. 
add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION) # Enables linking USD libraries with C++17

include(cmake/createTarget.cmake)

set(BUILD_USD_PLUGIN 1)
set(BUILD_HOUDINI_PLUGIN 0)

set(BUILD_ALL 0)
set(BUILD_HAIRPROC 0)
set(BUILD_CODEPROC 1)

if ("${BUILD_HAIRPROC}" OR "${BUILD_ALL}")
    add_subdirectory(hairProc)
endif()

if ("${BUILD_CODEPROC}" OR "${BUILD_ALL}")
    add_subdirectory(codeProc)
endif()

install(
    FILES __packageinit__.py
    DESTINATION lib/python/${args_PYPACKAGE_NAME}
    RENAME __init__.py
)

