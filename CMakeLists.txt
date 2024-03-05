cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(USD-resources VERSION 0.1.0 LANGUAGES CXX)

# Some RPATH stuff. Don't think this is how you do it....
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Set some compile definitions. 
add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION) # Enables linking USD libraries with C++17

include(cmake/createTarget.cmake)

set(BUILD_ALL 1)

if ("${BUILD_HAIRPROC}" OR "${BUILD_ALL}")
    add_subdirectory(hairProc)
endif()

