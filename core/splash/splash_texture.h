#pragma once

#include <string>
#include <bgfx/bgfx.h>

class SplashTexture {
public:
    SplashTexture();
    ~SplashTexture();

    bool Load(const std::string& path, void* assetManager = nullptr);
    void Destroy();

    bgfx::TextureHandle handle;
    int width = 0;
    int height = 0;
};