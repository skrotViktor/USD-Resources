# Copyright 2024 viktorlanner
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     https://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# Set the C++ requirements
set(CMAKE_CXX_STANDARD 17) # c++ 17 used for some convenience functions
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Target names
set(USDPLUGIN_NAME codeProc) # Need to match schema library name

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
add_subdirectory(gui)
add_subdirectory(ocl)
