// core/renderer/text_renderer.cpp
#include "text_renderer.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include "../logger.h"
#include <vector>
#include <fstream>

#ifdef __ANDROID__
#include <android/asset_manager.h>
#endif

bgfx::VertexLayout TextVertex::layout;

void TextVertex::init() {
    layout.begin()
          .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
          .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
          .end();
}

static const char* vsh_text =
    "#version 300 es\n"
    "layout(location=0) in vec2 aPos;\n"
    "layout(location=1) in vec2 aTexCoord;\n"
    "layout(location=2) in vec4 aColor;\n"
    "out vec2 v_texcoord;\n"
    "out vec4 v_color;\n"
    "uniform mat4 u_textProj;\n"
    "void main() {\n"
    "    gl_Position = u_textProj * vec4(aPos, 0.0, 1.0);\n"
    "    v_texcoord = aTexCoord;\n"
    "    v_color = aColor;\n"
    "}\n";

static const char* fsh_text =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_texcoord;\n"
    "in vec4 v_color;\n"
    "uniform sampler2D s_fontSampler;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    float alpha = texture(s_fontSampler, v_texcoord).a;\n"
    "    fragColor = vec4(v_color.rgb, v_color.a * alpha);\n"
    "}\n";

// Helper idéntico al de SplashRenderer para empaquetar shaders
static const bgfx::Memory* createShaderMem(const char* source, bool isVertex, uint16_t uniformCount = 0) {
    uint32_t sourceLen = (uint32_t)strlen(source);
    uint32_t headerLen = 10;
    uint32_t metadataLen = 0;
    if (!isVertex && uniformCount == 1) {
        metadataLen = 1 + 13 + 1 + 1 + 2 + 2; // "s_fontSampler"
    } else if (isVertex && uniformCount == 1) {
        metadataLen = 1 + 10 + 1 + 1 + 2 + 2; // "u_textProj"
    }

    uint32_t shaderSizeFieldLen = 4;
    const bgfx::Memory* mem = bgfx::alloc(headerLen + metadataLen + shaderSizeFieldLen + sourceLen);
    uint8_t* data = mem->data;
    memcpy(data, isVertex ? "VSH" : "FSH", 3);
    data[3] = 0x05;
    memset(data + 4, 0, 4);
    data[8] = (uint8_t)(uniformCount & 0xFF);
    data[9] = (uint8_t)((uniformCount >> 8) & 0xFF);

    uint8_t* curr = data + 10;
    if (!isVertex && uniformCount == 1) {
        const char* name = "s_fontSampler";
        uint8_t nameLen = (uint8_t)strlen(name);
        *curr++ = nameLen;
        memcpy(curr, name, nameLen); curr += nameLen;
        *curr++ = (uint8_t)bgfx::UniformType::Sampler;
        *curr++ = 1; *curr++ = 0; *curr++ = 0; *curr++ = 1; *curr++ = 0;
    } else if (isVertex && uniformCount == 1) {
        const char* name = "u_textProj";
        uint8_t nameLen = (uint8_t)strlen(name);
        *curr++ = nameLen;
        memcpy(curr, name, nameLen); curr += nameLen;
        *curr++ = (uint8_t)bgfx::UniformType::Mat4;
        *curr++ = 1; *curr++ = 0; *curr++ = 0; *curr++ = 1; *curr++ = 0;
    }

    *curr++ = (uint8_t)(sourceLen & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 8) & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 16) & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 24) & 0xFF);
    memcpy(curr, source, sourceLen);
    return mem;
}

TextRenderer::TextRenderer() : m_fontTexture(BGFX_INVALID_HANDLE), m_shaderProgram(BGFX_INVALID_HANDLE), s_texColor(BGFX_INVALID_HANDLE), u_proj(BGFX_INVALID_HANDLE) {}
TextRenderer::~TextRenderer() { Shutdown(); }

bool TextRenderer::Init(const std::string& fontPath, float fontSize, void* assetManager) {
    m_fontSize = fontSize;
    TextVertex::init();
    CreateShaderProgram();
    s_texColor = bgfx::createUniform("s_fontSampler", bgfx::UniformType::Sampler);
    u_proj = bgfx::createUniform("u_textProj", bgfx::UniformType::Mat4);

    // Cargar archivo TTF
    std::vector<uint8_t> fontData;
#ifdef __ANDROID__
    if (assetManager) {
        AAssetManager* am = (AAssetManager*)assetManager;
        AAsset* asset = AAssetManager_open(am, fontPath.c_str(), AASSET_MODE_BUFFER);
        if (asset) {
            size_t size = AAsset_getLength(asset);
            fontData.resize(size);
            AAsset_read(asset, fontData.data(), size);
            AAsset_close(asset);
        }
    }
#else
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        fontData.resize(size);
        if (!file.read((char*)fontData.data(), size)) {
            fontData.clear();
        }
    }
#endif

    if (fontData.empty()) {
        LOGE("No se pudo cargar la fuente: %s", fontPath.c_str());
        return false;
    }

    // Bake font atlas
    m_textureWidth = 512;
    m_textureHeight = 512;
    std::vector<uint8_t> alphaData(m_textureWidth * m_textureHeight);
    stbtt_bakedchar bakedChars[96]; // ASCII 32..126

    stbtt_BakeFontBitmap(fontData.data(), 0, fontSize, alphaData.data(), m_textureWidth, m_textureHeight, 32, 96, bakedChars);

    // Convertir R8 a RGBA8 para máxima compatibilidad
    std::vector<uint32_t> rgbaData(m_textureWidth * m_textureHeight);
    for (size_t i = 0; i < alphaData.size(); ++i) {
        rgbaData[i] = (static_cast<uint32_t>(alphaData[i]) << 24) | 0x00ffffff;
    }

    m_fontTexture = bgfx::createTexture2D(
        (uint16_t)m_textureWidth,
        (uint16_t)m_textureHeight,
        false, 1,
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE,
        bgfx::copy(rgbaData.data(), (uint32_t)rgbaData.size() * 4)
    );

    for (int i = 0; i < 96; ++i) {
        GlyphInfo g;
        g.x0 = (float)bakedChars[i].x0 / m_textureWidth;
        g.y0 = (float)bakedChars[i].y0 / m_textureHeight;
        g.x1 = (float)bakedChars[i].x1 / m_textureWidth;
        g.y1 = (float)bakedChars[i].y1 / m_textureHeight;
        g.xoff = bakedChars[i].xoff;
        g.yoff = bakedChars[i].yoff;
        g.xadvance = bakedChars[i].xadvance;
        g.width = bakedChars[i].x1 - bakedChars[i].x0;
        g.height = bakedChars[i].y1 - bakedChars[i].y0;
        m_glyphs.push_back(g);
    }

    return true;
}

void TextRenderer::CreateShaderProgram() {
    bgfx::ShaderHandle vsh = bgfx::createShader(createShaderMem(vsh_text, true, 1));
    bgfx::ShaderHandle fsh = bgfx::createShader(createShaderMem(fsh_text, false, 1));
    m_shaderProgram = bgfx::createProgram(vsh, fsh, true);
}

void TextRenderer::Shutdown() {
    if (bgfx::isValid(m_fontTexture)) bgfx::destroy(m_fontTexture);
    if (bgfx::isValid(m_shaderProgram)) bgfx::destroy(m_shaderProgram);
    if (bgfx::isValid(s_texColor)) bgfx::destroy(s_texColor);
    if (bgfx::isValid(u_proj)) bgfx::destroy(u_proj);
    m_fontTexture = BGFX_INVALID_HANDLE;
    m_shaderProgram = BGFX_INVALID_HANDLE;
}

void TextRenderer::RenderText(const std::string& text, float x, float y, uint32_t color, bgfx::ViewId viewId) {
    if (!bgfx::isValid(m_fontTexture) || !bgfx::isValid(m_shaderProgram)) return;

    std::vector<TextVertex> vertices;
    float curX = x;
    float curY = y;

    for (char c : text) {
        if (c < 32 || c > 126) continue;
        const GlyphInfo& g = m_glyphs[c - 32];

        float x0 = curX + g.xoff;
        float y0 = curY + g.yoff;
        float x1 = x0 + g.width;
        float y1 = y0 + g.height;

        vertices.push_back({x0, y0, g.x0, g.y0, color});
        vertices.push_back({x1, y0, g.x1, g.y0, color});
        vertices.push_back({x0, y1, g.x0, g.y1, color});

        vertices.push_back({x1, y0, g.x1, g.y0, color});
        vertices.push_back({x1, y1, g.x1, g.y1, color});
        vertices.push_back({x0, y1, g.x0, g.y1, color});

        curX += g.xadvance;
    }

    if (vertices.empty()) return;

    // Projection matrix (Screen space)
    float proj[16];
    bx::mtxOrtho(proj, 0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setUniform(u_proj, proj);

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, (uint32_t)vertices.size(), TextVertex::layout);

    memcpy(tvb.data, vertices.data(), vertices.size() * sizeof(TextVertex));
    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setTexture(0, s_texColor, m_fontTexture);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
    bgfx::submit(viewId, m_shaderProgram);
}
