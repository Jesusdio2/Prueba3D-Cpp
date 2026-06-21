# core/bgfx_cmake.cmake
# Manual definition of bgfx targets since it doesn't provide CMakeLists.txt

# --- BX ---
if(NOT bx_SOURCE_DIR)
    message(FATAL_ERROR "bx_SOURCE_DIR is not set. FetchContent failed?")
endif()

add_library(bx STATIC
        "${bx_SOURCE_DIR}/src/amalgamated.cpp"
)
target_include_directories(bx PUBLIC
        "${bx_SOURCE_DIR}/include"
        "${bx_SOURCE_DIR}/3rdparty"
)

# Inyectamos las definiciones de plataforma específicas para que el código amalgamado responda
target_compile_definitions(bx PUBLIC
        __STDC_FORMAT_MACROS

        # Si estamos en macOS/iOS (Darwin), forzamos las macros nativas de Apple y POSIX
        $<$<PLATFORM_ID:Darwin>:BX_PLATFORM_OSX=1>
        $<$<PLATFORM_ID:Darwin>:BX_CRT_NONE=1> # <- Forzar 1 le dice a bx que use el comportamiento POSIX puro libre de malloc.h
        $<$<PLATFORM_ID:Darwin>:__APPLE__=1>

        # Si estás en Android, las que correspondan a su NDK
        $<$<PLATFORM_ID:Android>:BX_PLATFORM_ANDROID=1>
        $<$<PLATFORM_ID:Android>:BX_CRT_NONE=0> # Android suele tener malloc.h, pero si falla, prueba 1
)

# --- BIMG ---
if(NOT bimg_SOURCE_DIR)
    message(FATAL_ERROR "bimg_SOURCE_DIR is not set. FetchContent failed?")
endif()

file(GLOB ASTC_SOURCES "${bimg_SOURCE_DIR}/3rdparty/astc-encoder/source/*.cpp")

add_library(bimg STATIC
    "${bimg_SOURCE_DIR}/src/image.cpp"
    "${bimg_SOURCE_DIR}/src/image_decode.cpp"
    "${bimg_SOURCE_DIR}/src/image_encode.cpp"
    "${bimg_SOURCE_DIR}/src/image_gnf.cpp"
    ${ASTC_SOURCES}
)
target_include_directories(bimg PUBLIC
    "${bimg_SOURCE_DIR}/include"
    "${bimg_SOURCE_DIR}/3rdparty"
    "${bimg_SOURCE_DIR}/3rdparty/astc-encoder/include"
    "${bimg_SOURCE_DIR}/3rdparty/iqa/include"
    "${bimg_SOURCE_DIR}/3rdparty/tinyexr/deps"
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
    $<$<PLATFORM_ID:Windows>:BGFX_CONFIG_RENDERER_DIRECT3D11=1>
)

target_link_libraries(bgfx PUBLIC bx bimg)
