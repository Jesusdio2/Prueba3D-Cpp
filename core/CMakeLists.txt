# core/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

add_library(core STATIC
        game.cpp
        scene.cpp
        game.h
        scene.h
        renderer/renderer.h
        renderer/text_renderer.cpp
        renderer/text_renderer.h
        splash/splash_renderer.cpp
        splash/splash_texture.cpp
        splash/splash_texture.h
        splash/splash_renderer.h
        stb_image.h
)

# Directorios de include locales del motor
target_include_directories(core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 🔥 CAMBIAR A C++20 (bgfx lo requiere)
target_compile_features(core PUBLIC cxx_std_20)

# 🔥 DEFINICIONES NECESARIAS PARA BGFX
target_compile_definitions(core PUBLIC
        $<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>
        $<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>
        BX_CONFIG_DEBUG=1  # Forzar para desarrollo
)

# 🎯 INCLUIR TARGETS MANUALES DE BGFX
include(bgfx_cmake.cmake)

# 🎯 LINK CON BGFX (Consumiendo los targets nativos descargados por la raíz)
target_link_libraries(core PUBLIC
        bgfx
        bx
        bimg
)

# 🎯 ANDROID OPENGL ES LINKING
if(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(egl-lib EGL)
    find_library(gles-lib GLESv3)

    target_link_libraries(core PRIVATE
            ${log-lib}
            ${android-lib}
            ${egl-lib}
            ${gles-lib}
    )
endif()

# 🎯 AÑADIR DIRECTORIOS DE INCLUDE DE BGFX
# Gracias a FetchContent, estas variables (_SOURCE_DIR) apuntan exactamente a las descargas
target_include_directories(core PUBLIC
        ${bx_SOURCE_DIR}/include
        ${bimg_SOURCE_DIR}/include
        ${bgfx_SOURCE_DIR}/include
        ${bgfx_SOURCE_DIR}/3rdparty
)

# 🎯 DEFINES DE PLATAFORMA (¡Añadido soporte explícito para Apple!)
target_compile_definitions(core PUBLIC
        $<$<PLATFORM_ID:Windows>:WINDOWS_PLATFORM>
        $<$<PLATFORM_ID:Android>:ANDROID_PLATFORM>
        $<$<PLATFORM_ID:Darwin>:APPLE_PLATFORM>
        $<$<PLATFORM_ID:iOS>:APPLE_PLATFORM>
)
