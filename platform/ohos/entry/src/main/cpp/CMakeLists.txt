# platform/ohos/entry/src/main/cpp/CMakeLists.txt
cmake_minimum_required(VERSION 3.16.5)
project(Prueba3DOHOS LANGUAGES CXX)

# 1. Heredar estándar C++20 requerido por tu core y bx/bgfx
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definiciones de preprocesador necesarias para entornos POSIX/Linux (como HarmonyOS)
add_compile_definitions(__STDC_FORMAT_MACROS)
add_compile_definitions(BX_CRT_NONE=0)
add_compile_definitions(OHOS_PLATFORM) # Definición útil por si necesitas segmentar código en el core

# 2. El objetivo principal: El puente NAPI nativo de Huawei (.so)
add_library(entry SHARED
    napi_init.cpp # <--- CAMBIADO AQUÍ: Tu archivo real descubierto con ls
)

# 3. ENLAZAR TU LOGICA COMPARTIDA DE CORE Y DEPENDENCIAS MULTIPLATAFORMA
# (El target "core" viene inyectado automáticamente desde el CMake raíz global)
target_link_libraries(entry PRIVATE
    core
    bgfx
    bimg
    bx
)

# 4. Incluir cabeceras de la API de Huawei y el directorio core para resolver los archivos .h
target_include_directories(entry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../core
)

# Enlazar librerías de bajo nivel nativas del sistema operativo de Huawei
target_link_libraries(entry PRIVATE 
    ace_napi.z
    hilog_ndk.z
)
