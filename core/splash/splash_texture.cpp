#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "splash_texture.h"
#include <bx/bx.h>
#include <android/log.h>
#include <iostream>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SplashTexture", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SplashTexture", __VA_ARGS__)

#include <android/asset_manager.h>

SplashTexture::SplashTexture()
        : handle(BGFX_INVALID_HANDLE)
        , width(0)
        , height(0) {
}

SplashTexture::~SplashTexture() {
    Destroy();
}

bool SplashTexture::Load(const std::string& path, void* assetManager) {
    int channels;
    unsigned char* data = nullptr;

    if (assetManager) {
        AAssetManager* am = (AAssetManager*)assetManager;
        AAsset* asset = AAssetManager_open(am, path.c_str(), AASSET_MODE_BUFFER);
        if (asset) {
            off_t length = AAsset_getLength(asset);
            const void* buffer = AAsset_getBuffer(asset);
            data = stbi_load_from_memory((const stbi_uc*)buffer, (int)length, &width, &height, &channels, 4);
            AAsset_close(asset);
        } else {
            LOGE("No se pudo abrir el asset: %s", path.c_str());
            return false;
        }
    } else {
        data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    }

    if (!data) {
        LOGE("Error cargando splash: %s", path.c_str());
        return false;
    }

    // Crear textura en bgfx desde los datos en memoria
    const bgfx::Memory* mem = bgfx::copy(data, width * height * 4);

    // Crear la textura con formato RGBA8
    handle = bgfx::createTexture2D(
            (uint16_t)width,
            (uint16_t)height,
            false,  // No tiene mipmaps
            1,      // Número de layers
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,
            mem
    );

    stbi_image_free(data);

    if (!bgfx::isValid(handle)) {
        LOGE("Error creando textura en bgfx");
        return false;
    }

    LOGI("Splash cargado: %dx%d", width, height);
    return true;
}

void SplashTexture::Destroy() {
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
        handle = BGFX_INVALID_HANDLE;
    }
    width = 0;
    height = 0;
}