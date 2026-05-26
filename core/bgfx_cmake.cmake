# core/bgfx_cmake.cmake
# Manual definition of bgfx targets since it doesn't provide CMakeLists.txt

# --- BX ---
add_library(bx STATIC
    ${bx_SOURCE_DIR}/src/amalgamated.cpp
)
target_include_directories(bx PUBLIC
    ${bx_SOURCE_DIR}/include
    ${bx_SOURCE_DIR}/3rdparty
)
target_compile_definitions(bx PUBLIC
    __STDC_FORMAT_MACROS
)

# --- BIMG ---
file(GLOB ASTC_SOURCES "${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.cpp")

add_library(bimg STATIC
    ${bimg_SOURCE_DIR}/src/image.cpp
    ${bimg_SOURCE_DIR}/src/image_decode.cpp
    ${bimg_SOURCE_DIR}/src/image_encode.cpp
    ${bimg_SOURCE_DIR}/src/image_gnf.cpp
    ${ASTC_SOURCES}
)
target_include_directories(bimg PUBLIC
    ${bimg_SOURCE_DIR}/include
    ${bimg_SOURCE_DIR}/3rdparty
    ${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include
    ${bimg_SOURCE_DIR}/3rdparty/iqa/include
    ${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps
)
target_link_libraries(bimg PUBLIC bx)

# --- BGFX ---
add_library(bgfx STATIC
    ${bgfx_SOURCE_DIR}/src/amalgamated.cpp
)

target_include_directories(bgfx PUBLIC
    ${bgfx_SOURCE_DIR}/include
    ${bgfx_SOURCE_DIR}/3rdparty
    ${bgfx_SOURCE_DIR}/3rdparty/khronos
)

target_compile_definitions(bgfx PUBLIC
    $<$<PLATFORM_ID:Android>:BGFX_CONFIG_RENDERER_OPENGLES=30>
    $<$<PLATFORM_ID:Darwin>:BGFX_CONFIG_RENDERER_METAL=1>
)

target_link_libraries(bgfx PUBLIC bx bimg)
