# platform/apple/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Prueba3DApple LANGUAGES CXX OBJCXX)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(APPLE)
    add_compile_definitions(APPLE_PLATFORM)

    if(IOS)
        add_compile_definitions(TARGET_OS_IOS)
    else()
        add_compile_definitions(TARGET_OS_MAC)
    endif()
endif()

message(STATUS "Configurando entorno Apple. Los binarios estáticos se guardarán en: ${CMAKE_CURRENT_BINARY_DIR}")

# ------------------------------------------------------------------
# 🚀 FLUJO PARA XCODE (Tu configuración original)
# ------------------------------------------------------------------
if(XCODE)
    add_custom_target(Auto_Compilar_Core ALL
            COMMAND /opt/homebrew/bin/cmake --build ${CMAKE_BINARY_DIR} --config $<CONFIG>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "CMake: Recompilando el motor C++ de forma automatizada antes de lanzar la App..."
    )

    # ------------------------------------------------------------------
    # ⚡ FLUJO PARA RIDER / CMAKE NATIVO (Lo que le falta a Rider)
    # ------------------------------------------------------------------
else()
    # 1. Buscamos tus archivos nativos de la capa de Mac (.mm, .cpp, .h) que usas en tu app
    file(GLOB_RECURSE MAC_NATIVE_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    )

    # Si tienes tus archivos de interfaz de Mac en esta carpeta, creamos el binario para Rider
    if(MAC_NATIVE_SOURCES)
        add_executable(Prueba3D_Mac ${MAC_NATIVE_SOURCES})

        # Enlazamos tu core (que ya incluye bgfx)
        target_link_libraries(Prueba3D_Mac PRIVATE core)

        # Frameworks obligatorios de Apple para que tu main/bgfx compilen en Mac
        target_link_libraries(Prueba3D_Mac PRIVATE
                "-framework Cocoa"
                "-framework Metal"
                "-framework QuartzCore"
                "-framework IOKit"
        )

        # Le decimos a macOS que genere un contenedor .app ejecutable
        set_target_properties(Prueba3D_Mac PROPERTIES
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_BUNDLE_NAME "Prueba3D"
        )
    endif()
endif()