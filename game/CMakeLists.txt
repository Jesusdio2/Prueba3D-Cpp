cmake_minimum_required(VERSION 3.22)
project(MyCrossGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_WINDOWS "Build Windows" ON)
option(BUILD_MACOS "Build macOS" ON)
option(BUILD_ANDROID "Build Android" ON)

add_subdirectory(external)
add_subdirectory(engine)

add_executable(GameApp src/main.cpp)
target_link_libraries(GameApp PRIVATE engine_core engine_platform engine_render)

# Selección de backend por plataforma
if(WIN32)
  target_compile_definitions(GameApp PRIVATE RENDER_D3D12)
  target_link_libraries(GameApp PRIVATE engine_render_d3d12)
elseif(APPLE)
  target_compile_definitions(GameApp PRIVATE RENDER_METAL)
  target_link_libraries(GameApp PRIVATE engine_render_metal)
elseif(ANDROID)
  target_compile_definitions(GameApp PRIVATE RENDER_VULKAN)
  target_link_libraries(GameApp PRIVATE engine_render_vulkan)
endif()
