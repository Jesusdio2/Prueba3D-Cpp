# core/CMakeLists.txt
cmake_minimum_required(VERSION 3.18)

# Crea una librería que pueda ser usada por Android y UWP
# Agregamos todos los archivos fuente del core
add_library(core STATIC
        game.cpp
        scene.cpp
        platform/opengl/opengl_renderer.cpp
        platform/opengl/opengl_renderer.h
        game.h
        scene.h
        renderer/renderer.h
)

# Incluye headers de core/
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Opcional: define macros según plataforma
target_compile_definitions(core PUBLIC
        $<$<PLATFORM_ID:Windows>:WINDOWS_PLATFORM>
        $<$<PLATFORM_ID:Android>:ANDROID_PLATFORM>
)