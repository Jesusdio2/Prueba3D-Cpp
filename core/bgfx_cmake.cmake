# core/bgfx_cmake.cmake
# Manual definition of bgfx targets since it doesn't provide CMakeLists.txt

# --- BX ---
if(NOT bx_SOURCE_DIR)
    message(FATAL_ERROR "bx_SOURCE_DIR is not set. FetchContent failed?")
endif()

file(GLOB BX_SOURCES "${bx_SOURCE_DIR}/src/*.cpp")
list(FILTER BX_SOURCES EXCLUDE REGEX "amalgamated\\.cpp$")
list(FILTER BX_SOURCES EXCLUDE REGEX "crtnone\\.cpp$")

add_library(bx STATIC ${BX_SOURCES})

target_include_directories(bx PUBLIC
        "${bx_SOURCE_DIR}/include"
        "${bx_SOURCE_DIR}/3rdparty"
)

if(MINGW)
    target_include_directories(bx BEFORE PUBLIC
            "${bx_SOURCE_DIR}/include/compat/mingw"
    )
    target_compile_definitions(bx PUBLIC
            MINGW_HAS_SECURE_API=1
    )
    target_compile_options(bx PUBLIC
            -msse4.1
    )
endif()

# Dejamos que bx detecte la plataforma automáticamente.
# Las definiciones globales (__STDC_FORMAT_MACROS, BX_CONFIG_DEBUG) ya vienen del CMakeLists.txt raíz.

# --- BIMG ---
if(NOT bimg_SOURCE_DIR)
    message(FATAL_ERROR "bimg_SOURCE_DIR is not set. FetchContent failed?")
endif()

file(GLOB ASTC_SOURCES "${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.cpp")

add_library(bimg STATIC
    "${bimg_SOURCE_DIR}/src/image.cpp"
    "${bimg_SOURCE_DIR}/src/image_decode.cpp"
    "${bimg_SOURCE_DIR}/src/image_encode.cpp"
    "${bimg_SOURCE_DIR}/src/image_cubemap_filter.cpp"
    ${ASTC_SOURCES}
)
target_include_directories(bimg PUBLIC
    "${bimg_SOURCE_DIR}/include"
    "${bimg_SOURCE_DIR}/3rdparty"
    "${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include"
    "${bimg_SOURCE_DIR}/3rdparty/iqa/include"
    "${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps"
)
# Deshabilitamos formatos que requieren dependencias externas complejas (avif, heif)
target_compile_definitions(bimg PRIVATE
    BIMG_CONFIG_PARSE_AVIF=0
    BIMG_CONFIG_PARSE_HEIF=0
)
target_link_libraries(bimg PUBLIC bx)

# --- BGFX ---
if(NOT bgfx_SOURCE_DIR)
    message(FATAL_ERROR "bgfx_SOURCE_DIR is not set. FetchContent failed?")
endif()

add_library(bgfx STATIC
    "${bgfx_SOURCE_DIR}/src/amalgamated.cpp"
)

target_include_directories(bgfx PUBLIC
    "${bgfx_SOURCE_DIR}/include"
    "${bgfx_SOURCE_DIR}/3rdparty"
    "${bgfx_SOURCE_DIR}/3rdparty/khronos"
)

target_compile_definitions(bgfx PUBLIC
    $<$<PLATFORM_ID:Android>:BGFX_CONFIG_RENDERER_OPENGLES=30>
    $<$<PLATFORM_ID:Darwin>:BGFX_CONFIG_RENDERER_METAL=1>
    $<$<PLATFORM_ID:Windows>:BGFX_CONFIG_RENDERER_OPENGL=41>
    $<$<PLATFORM_ID:Windows>:BGFX_CONFIG_RENDERER_DIRECT3D11=0>
    $<$<PLATFORM_ID:Windows>:BGFX_CONFIG_RENDERER_DIRECT3D12=0>
    $<$<BOOL:${WIN32}>:BGFX_CONFIG_RENDERER_OPENGL=41>
    $<$<BOOL:${WIN32}>:BGFX_CONFIG_RENDERER_DIRECT3D11=0>
    $<$<BOOL:${WIN32}>:BGFX_CONFIG_RENDERER_DIRECT3D12=0>
)

target_link_libraries(bgfx PUBLIC bx bimg)
