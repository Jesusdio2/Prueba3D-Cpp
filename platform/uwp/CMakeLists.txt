# UWP app con XAML
project(Prueba3DUWP LANGUAGES CXX)

# Crea el ejecutable
add_executable(Prueba3DUWP
        Main.cpp            # Archivo que lanza tu app UWP
)

# Enlaza la librería core
target_link_libraries(Prueba3DUWP PRIVATE core)

# Incluye headers de core
target_include_directories(Prueba3DUWP PRIVATE ${CMAKE_SOURCE_DIR}/core)

# Macro para UWP
target_compile_definitions(Prueba3DUWP PRIVATE WINDOWS_PLATFORM)

# Habilita C++17
set_target_properties(Prueba3DUWP PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
)

# Configuración específica de UWP
set_target_properties(Prueba3DUWP PROPERTIES
        WIN32_EXECUTABLE TRUE
)