#pragma once

#include <bgfx/bgfx.h>
#include <string>
#include <vector>

struct TextVertex {
    float x, y;
    float u, v;
    uint32_t color;

    static bgfx::VertexLayout layout;
    static void init();
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool Init(const std::string& fontPath, float fontSize, void* assetManager = nullptr);
    void Shutdown();

    void RenderText(const std::string& text, float x, float y, uint32_t color, bgfx::ViewId viewId);

private:
    bgfx::TextureHandle m_fontTexture;
    bgfx::ProgramHandle m_shaderProgram;
    bgfx::UniformHandle s_texColor;
    bgfx::UniformHandle u_proj;

    // Almacenamos los datos de los glifos de stb_truetype
    struct GlyphInfo {
        float x0, y0, x1, y1; // Coordenadas en la textura (UV)
        float xoff, yoff, xadvance;
        int width, height;
    };
    std::vector<GlyphInfo> m_glyphs;

    float m_fontSize;
    int m_textureWidth;
    int m_textureHeight;

    void CreateShaderProgram();
};
