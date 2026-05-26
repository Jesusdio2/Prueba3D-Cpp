cmake_minimum_required(VERSION 3.20)
project(Prueba3D LANGUAGES CXX OBJCXX)

# 🔥 ELIMINADO EL C++20 GLOBAL PARA QUE NO ROMPA BGFX/BX

# 🔥 DEFINICIONES GLOBALES PARA BGFX
add_compile_definitions(BX_CONFIG_DEBUG=1)

# 1. Configurar y Descargar dependencias oficiales de Branimir Karadžić
include(FetchContent)

FetchContent_Declare(
        bx
        GIT_REPOSITORY https://github.com/bkaradzic/bx.git
        GIT_TAG        master
)
FetchContent_Declare(
        bimg
        GIT_REPOSITORY https://github.com/bkaradzic/bimg.git
        GIT_TAG        master
)
FetchContent_Declare(
        bgfx
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.git
        GIT_TAG        master
)

# Descarga moderna y limpia recomendada por CMake
FetchContent_MakeAvailable(bx bimg bgfx)

# 🛠️ CONECTAR LOS SCRIPT DE CMAKE OFICIALES DE BGFX:
add_subdirectory(${bx_SOURCE_DIR}/cmake ${bx_BINARY_DIR}_build)
add_subdirectory(${bgfx_SOURCE_DIR}/cmake ${bgfx_BINARY_DIR}_build)

# 2. Configurar el CORE (Tu motor - Aquí adentro sí se fuerza C++20 de forma aislada)
add_subdirectory(core)

# 3. Detectar Plataforma
if (ANDROID)
    message(STATUS "Configurando para Android")
    add_subdirectory(platform/android)

elseif (WIN32)
    message(STATUS "Configurando para Windows/UWP")
    add_subdirectory(platform/uwp)

elseif (APPLE)
    message(STATUS "Configurando para Apple")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(platform/apple)
endif()
