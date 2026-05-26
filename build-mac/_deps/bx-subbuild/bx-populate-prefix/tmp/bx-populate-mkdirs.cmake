# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-src")
  file(MAKE_DIRECTORY "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-src")
endif()
file(MAKE_DIRECTORY
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-build"
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix"
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/tmp"
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/src/bx-populate-stamp"
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/src"
  "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/src/bx-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/src/bx-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/Francisco/AndroidStudioProjects/Prueba3D-Cpp/build-mac/_deps/bx-subbuild/bx-populate-prefix/src/bx-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
