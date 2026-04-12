#pragma once

#include "splash_texture.h"

class SplashRenderer {
public:
    SplashRenderer();
    ~SplashRenderer();

    void Init();
    void Render(SplashTexture& tex);

private:
    unsigned int shaderProgram = 0;
    unsigned int vbo = 0;
    unsigned int vao = 0;

    unsigned int CompileShader(unsigned int type, const char* source);
    unsigned int CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
};