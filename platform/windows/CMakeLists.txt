cmake_minimum_required(VERSION 3.20)
project(Prueba3D_Windows CXX)

# WIN32 es necesario para que el enlazador reconozca WinMain como punto de entrada GUI
add_executable(prueba3d_win WIN32
    main.cpp
)

# Forzar C++20 para la plataforma Windows
set_property(TARGET prueba3d_win PROPERTY CXX_STANDARD 20)
set_property(TARGET prueba3d_win PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(prueba3d_win PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core
)

target_compile_definitions(prueba3d_win PRIVATE
    WINDOWS_PLATFORM
)

target_link_libraries(prueba3d_win PRIVATE
    core
    bgfx
    bx
    bimg
    gdi32
    user32
    opengl32
)

if(MINGW)
    target_link_options(prueba3d_win PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Copiar el contenido de assets directamente al directorio de salida
add_custom_command(TARGET prueba3d_win POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/core/assets"
    "$<TARGET_FILE_DIR:prueba3d_win>"
)

# Cambiar el directorio de trabajo para que cargue los assets correctamente si se usa ruta relativa
set_target_properties(prueba3d_win PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:prueba3d_win>")

