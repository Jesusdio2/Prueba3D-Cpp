cmake_minimum_required(VERSION 3.18)

add_library(core STATIC
        game.cpp
        scene.cpp
        platform/opengl/opengl_renderer.cpp
        platform/opengl/opengl_renderer.h
        game.h
        scene.h
        renderer/renderer.h
        splash/splash_renderer.cpp
        splash/splash_texture.cpp   # 🔥 AQUI
        splash/splash_texture.h
        splash/splash_renderer.h
        stb_image.h
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 🎯 ANDROID OPENGL ES LINKING
if(ANDROID)

    find_library(log-lib log)
    find_library(android-lib android)
    find_library(egl-lib EGL)
    find_library(gles-lib GLESv3)

    target_link_libraries(core
            ${log-lib}
            ${android-lib}
            ${egl-lib}
            ${gles-lib}
    )

endif()

# 🎯 DEFINES DE PLATAFORMA
target_compile_definitions(core PUBLIC
        $<$<PLATFORM_ID:Windows>:WINDOWS_PLATFORM>
        $<$<PLATFORM_ID:Android>:ANDROID_PLATFORM>
)