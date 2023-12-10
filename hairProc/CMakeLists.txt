cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hairProc VERSION 0.1.0 LANGUAGES CXX)

set(BUILD_USD_PLUGIN 1)
set(BUILD_HOUDINI_PLUGIN 1)

# Set the C++ requirements
set(CMAKE_CXX_STANDARD 17) # c++ 17 used for some convenience functions
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Target names
set(USDPLUGIN_NAME hairProc) # Need to match schema library name
set(PYPACKAGE_NAME vik) # The package name used for python. "from vik import HairProc"
set(OCLMODULE_NAME ocl) # The name of the opencl library

# Install directory
set(USD_INSTALL_ROOT /opt/USD) # The path to our USD istall directory

set(CMAKE_INSTALL_PREFIX "/opt/USD_resources" CACHE PATH "..." FORCE) # Where this project will be installed

# Some RPATH stuff. Don't think this is how you do it....
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Set some compile definitions. 
add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION) # Enables linking USD libraries with C++17

include(cmake/createTarget.cmake)

# The subdirectories defines our actual library targets
add_subdirectory(usd)
add_subdirectory(ocl)
