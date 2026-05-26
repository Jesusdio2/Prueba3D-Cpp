#pragma once

#include "splash_texture.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>

class SplashRenderer {
public:
    SplashRenderer();
    ~SplashRenderer();

    void Init();
    void Shutdown();
    void Render(SplashTexture& tex, bgfx::ViewId viewId = 0);

private:
    void CreateShaderProgram();

    bgfx::ProgramHandle shaderProgram;
    bgfx::VertexBufferHandle vbh;
    bgfx::VertexLayout layout;
    bgfx::UniformHandle s_texColor;  // Uniform para el sampler de textura

    bool isValid;
};