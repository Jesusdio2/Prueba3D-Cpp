# cmakelists.txt (Raiz)
cmake_minimum_required(VERSION 3.20)
project(Prueba3D LANGUAGES CXX)

# 1. 🔥 FORZAR ESTÁNDAR C++20 (Obligatorio para las versiones recientes de bgfx/bx)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. 🔥 SILENCIAR ADVERTENCIA DE FETCHCONTENT_POPULATE
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Si estamos en Apple, habilitamos Objective-C++
if (APPLE)
    enable_language(OBJCXX)
endif()

# 3. 🔥 DEFINICIONES GLOBALES PARA EL ECOSISTEMA BGFX
# BX y BIMG requieren BX_CONFIG_DEBUG. Lo sincronizamos con el tipo de build de CMake.
add_compile_definitions(BX_CONFIG_DEBUG=$<CONFIG:Debug>)
add_compile_definitions(__STDC_FORMAT_MACROS)

if (APPLE)
    add_compile_options(-fPIC)
endif()

# 4. Configurar y Descargar dependencias oficiales
include(FetchContent)

# Definir una carpeta base para FetchContent para evitar descargas duplicadas por cada ABI
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps" CACHE PATH "Shared FetchContent base directory")

FetchContent_Declare(
        bx
        GIT_REPOSITORY https://github.com/bkaradzic/bx.git
        GIT_TAG master
)

FetchContent_Declare(
        bimg
        GIT_REPOSITORY https://github.com/bkaradzic/bimg.git
        GIT_TAG master
)

FetchContent_Declare(
        bgfx
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.git
        GIT_TAG master
)

# Descargamos los archivos pero NO intentamos usar sus CMakeLists (porque no tienen en la raíz)
message(STATUS "Populating dependencies...")

FetchContent_GetProperties(bx)
if(NOT bx_POPULATED)
    FetchContent_Populate(bx)
endif()

FetchContent_GetProperties(bimg)
if(NOT bimg_POPULATED)
    FetchContent_Populate(bimg)
endif()

FetchContent_GetProperties(bgfx)
if(NOT bgfx_POPULATED)
    FetchContent_Populate(bgfx)
endif()

message(STATUS "bx source: ${bx_SOURCE_DIR}")
message(STATUS "bimg source: ${bimg_SOURCE_DIR}")
message(STATUS "bgfx source: ${bgfx_SOURCE_DIR}")

if(NOT bx_SOURCE_DIR OR NOT bimg_SOURCE_DIR OR NOT bgfx_SOURCE_DIR)
    message(FATAL_ERROR "Failed to populate dependencies. Check your internet connection or GIT_TAG.")
endif()

# 5. Configurar el CORE (Éste usará bgfx_cmake.cmake para definir los targets)
add_subdirectory(core)

# 6. Detectar Plataforma
if (ANDROID)
    message(STATUS "Configurando para Android")
    add_subdirectory(platform/android)

elseif (OHOS) # <--- Todo junto: elseif
    message(STATUS "Configurando para Huawei / OpenHarmony")
    add_subdirectory(platform/ohos/entry/src/main/cpp)

elseif (APPLE)
    message(STATUS "Configurando para Apple")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(platform/apple)

elseif (WIN32)
    message(STATUS "Configurando para Windows/UWP")
    add_subdirectory(platform/windows)
endif()