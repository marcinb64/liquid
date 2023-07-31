cmake_minimum_required(VERSION 3.13)

# Project definition
project(
    liquid
    VERSION 0.3.2
    LANGUAGES CXX)

# Build customization
set(CMAKE_BUILD_TYPE Debug)
option(WARNINGS_AS_ERRORS On)
option(ENABLE_CLANG_TIDY Off)
option(ENABLE_SANITIZERS Off)

# Project-specific settings
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
SET(CMAKE_CXX_FLAGS_DEBUG "-O2 -g")

# Pseudo-library for adding compiler warning flags to targets
add_library(project_warnings INTERFACE)
include(CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Misc options
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT "${LIQUID_PLATFORM}" STREQUAL "avr")
    include(CTest)
endif ()

include(StaticAnalysis.cmake)

#
# Project structure
#
add_subdirectory(liquid)
if (NOT LIB_ONLY)
    add_subdirectory(demo)
endif ()
