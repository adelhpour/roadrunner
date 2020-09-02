# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget f2c blas lapack)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target f2c
add_library(f2c SHARED IMPORTED)

# Create imported target blas
add_library(blas SHARED IMPORTED)

set_target_properties(blas PROPERTIES
  INTERFACE_LINK_LIBRARIES "f2c"
)

# Create imported target lapack
add_library(lapack SHARED IMPORTED)

set_target_properties(lapack PROPERTIES
  INTERFACE_LINK_LIBRARIES "blas"
)

# Import target "f2c" for configuration "Debug"
set_property(TARGET f2c APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(f2c PROPERTIES
  IMPORTED_IMPLIB_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/lib/f2cd.lib"
  IMPORTED_LOCATION_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/bin/libf2cd.dll"
  )

# Import target "blas" for configuration "Debug"
set_property(TARGET blas APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(blas PROPERTIES
  IMPORTED_IMPLIB_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/lib/blasd.lib"
  IMPORTED_LOCATION_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/bin/blasd.dll"
  )

# Import target "lapack" for configuration "Debug"
set_property(TARGET lapack APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lapack PROPERTIES
  IMPORTED_IMPLIB_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/lib/lapackd.lib"
  IMPORTED_LOCATION_DEBUG "D:/rrbuild/roadrunner/out/build/x64-Debug/bin/lapackd.dll"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
