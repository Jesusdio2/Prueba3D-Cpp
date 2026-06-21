cmake_minimum_required(VERSION 3.20)
project(Prueba3D_Windows)

add_executable(prueba3d_win
    main.cpp
)

target_link_libraries(prueba3d_win
    core
    bgfx
    bx
    bimg
    gdi32
    user32
    d3d11
    dxgi
)

# Copiar el contenido de assets directamente al directorio de salida
add_custom_command(TARGET prueba3d_win POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/core/assets"
    "$<TARGET_FILE_DIR:prueba3d_win>"
)

# Cambiar el directorio de trabajo para que cargue los assets correctamente si se usa ruta relativa
set_target_properties(prueba3d_win PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:prueba3d_win>")
