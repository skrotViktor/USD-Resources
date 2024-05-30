set(USDPLUGIN_NAME hairProc) # Need to match schema library name

set(BUILD_USD_PLUGIN 1)
set(BUILD_HOUDINI_PLUGIN 1)

# Set the C++ requirements
set(CMAKE_CXX_STANDARD 17) # c++ 17 used for some convenience functions
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${BUILD_USD_PLUGIN} AND NOT DEFINED USD_INSTALL_ROOT)
    message(FATAL_ERROR "Missing USD_INSTALL_ROOT")
    return()
endif()

if (${BUILD_HOUDINI_PLUGIN} AND NOT DEFINED HOUDINI_INSTALL_ROOT)
    message(FATAL_ERROR "Missing HOUDINI_INSTALL_ROOT")
    return()
endif()

# The subdirectories defines our actual library targets
add_subdirectory(usd)
add_subdirectory(ocl)
