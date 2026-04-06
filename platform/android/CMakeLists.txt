# Android Platform CMake
cmake_minimum_required(VERSION 3.18)

# Creamos una librería dinámica (.so) para Android
add_library(prueba3d_native SHARED
        jni_bridge.cpp
)

# Enlazamos con la librería core STATIC
target_link_libraries(prueba3d_native
        core
        log # Para ver mensajes en Logcat
)
