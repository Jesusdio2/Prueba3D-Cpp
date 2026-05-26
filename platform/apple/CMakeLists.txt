# platform/apple/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Prueba3DApple LANGUAGES CXX OBJCXX)

# 1. Definimos una librería "intermedia" para Apple si fuera necesario,
# pero lo ideal es simplemente asegurar que 'core' compile con bgfx.
# Si 'core' ya está definido en la raíz, lo enlazamos aquí.

# 2. Forzar que las dependencias generen librerías estáticas (.a)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# 3. Configuraciones específicas para plataformas Apple (Metal, etc.)
if(APPLE)
    # Definición global para tu código C++ sepa que está en Apple
    add_compile_definitions(APPLE_PLATFORM)

    if(IOS)
        add_compile_definitions(TARGET_OS_IOS)
    elseif(XCODE)
        add_compile_definitions(TARGET_OS_MAC)
    endif()
endif()

# 4. Mensaje informativo para tu consola de CMake
message(STATUS "Configurando entorno Apple. Los binarios estáticos se guardarán en: ${CMAKE_CURRENT_BINARY_DIR}")

# 5. Target de automatización nativo para Xcode (Evita tocar la interfaz gráfica)
if(XCODE)
    add_custom_target(Auto_Compilar_Core ALL
            COMMAND /opt/homebrew/bin/cmake --build ${CMAKE_BINARY_DIR} --config $<CONFIG>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "CMake: Recompilando el motor C++ de forma automatizada antes de lanzar la App..."
    )
endif()
