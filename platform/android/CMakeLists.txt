# platform/android/CMakeLists.txt
cmake_minimum_required(VERSION 3.18)

project(Prueba3D_Android)

add_library(prueba3d_native SHARED
    jni_bridge.cpp
)

target_include_directories(prueba3d_native PRIVATE
    ${CMAKE_SOURCE_DIR}/core
)

# 🔥 DEFINICIONES NECESARIAS
target_compile_definitions(prueba3d_native PRIVATE
    ANDROID_PLATFORM
    BX_CONFIG_DEBUG=1
)

# 🔥 USAR C++20
set_property(TARGET prueba3d_native PROPERTY CXX_STANDARD 20)
set_property(TARGET prueba3d_native PROPERTY CXX_STANDARD_REQUIRED ON)

# Añadir includes de bgfx
target_include_directories(prueba3d_native PRIVATE
    ${bgfx_SOURCE_DIR}/include
    ${bx_SOURCE_DIR}/include
    ${bimg_SOURCE_DIR}/include
)

find_library(log-lib log)
find_library(android-lib android)
find_library(EGL_LIB EGL)
find_library(GLES_LIB GLESv3)

find_package(games-frame-pacing REQUIRED CONFIG)

target_link_libraries(prueba3d_native
    core
    bgfx
    bx
    bimg
    ${GLES_LIB}
    ${EGL_LIB}
    ${log-lib}
    ${android-lib}
    games-frame-pacing::swappy_static
)

# 🔥 FIX Android 15+
target_link_options(prueba3d_native PRIVATE
    "-Wl,-z,max-page-size=16384"
    "-Wl,-z,common-page-size=16384"
)

target_compile_options(prueba3d_native PRIVATE
    -O2
    -ffast-math
    -fno-exceptions
    -fno-rtti
)