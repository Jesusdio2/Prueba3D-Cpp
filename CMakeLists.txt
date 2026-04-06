cmake_minimum_required(VERSION 3.18)
project(Prueba3D)

# Primero, el core
add_subdirectory(core)

# Detecta la plataforma: ANDROID o UWP (WIN32)
if (ANDROID)
    message(STATUS "Configurando para Android")
    add_subdirectory(platform/android)
elseif (WIN32)
    message(STATUS "Configurando para UWP")
    add_subdirectory(platform/uwp)
endif()