cmake_minimum_required(VERSION 3.20)
project(Prueba3D LANGUAGES CXX)

# Si estamos en Apple, habilitamos Objective-C++
if (APPLE)
    enable_language(OBJCXX)
endif()

# 🔥 DEFINICIONES GLOBALES PARA BGFX
add_compile_definitions(BX_CONFIG_DEBUG=1)

# 1. Configurar y Descargar dependencias oficiales
include(FetchContent)

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
FetchContent_Populate(bx)
FetchContent_Populate(bimg)
FetchContent_Populate(bgfx)

# 2. Configurar el CORE (Éste usará bgfx_cmake.cmake para definir los targets)
add_subdirectory(core)

# 3. Detectar Plataforma
if (ANDROID)
    message(STATUS "Configurando para Android")
    add_subdirectory(platform/android)

elseif (APPLE)
    message(STATUS "Configurando para Apple")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(platform/apple)

elseif (WIN32)
    message(STATUS "Configurando para Windows/UWP")
    add_subdirectory(platform/uwp)
endif()
