# CMake para macOS
add_executable(Prueba3DMac main.cpp)

# Enlaza la librería core
target_link_libraries(Prueba3DMac PRIVATE core)

# Incluye headers
target_include_directories(Prueba3DMac PRIVATE ${CMAKE_SOURCE_DIR}/core)

# Macro opcional para Apple
target_compile_definitions(Prueba3DMac PRIVATE APPLE_PLATFORM)