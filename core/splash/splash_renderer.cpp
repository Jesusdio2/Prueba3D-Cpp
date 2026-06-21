#include "splash_renderer.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <bx/file.h>
#include <android/log.h>
#include <vector>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SplashRenderer", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SplashRenderer", __VA_ARGS__)

struct PosTexCoordVertex {
    float x, y;
    float u, v;

    static bgfx::VertexLayout ms_layout;
    static void init() {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        LOGI("VertexLayout initialized: stride %d, hash %u", ms_layout.m_stride, ms_layout.m_hash);
    }
};

bgfx::VertexLayout PosTexCoordVertex::ms_layout;

// Simple GLSL shaders for OpenGL ES 3.0
static const char* vertexShaderSource =
    "#version 300 es\n"
    "layout(location=0) in vec2 aPos;\n"
    "layout(location=1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

static const char* fragmentShaderSource =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

// Helper to wrap GLSL source into bgfx shader memory
static const bgfx::Memory* createShaderMem(const char* source, bool isVertex, uint16_t uniformCount = 0) {
    uint32_t sourceLen = (uint32_t)strlen(source);
    // Header: Magic(4), Hash(4), UniformCount(2) = 10 bytes
    uint32_t headerLen = 10;
    uint32_t metadataLen = 0;

    // If uniformCount > 0, we MUST provide metadata (name length, name string, type, num, regIndex, regCount)
    // for each uniform. For our fragment shader, we have 1 uniform: "ourTexture".
    if (!isVertex && uniformCount == 1) {
        // nameLen(1) + "ourTexture"(10) + type(1) + num(1) + regIndex(2) + regCount(2) = 17 bytes
        metadataLen = 1 + 10 + 1 + 1 + 2 + 2;
    }

    // After metadata, bgfx expects a 4-byte shader size (uint32_t)
    uint32_t shaderSizeFieldLen = 4;

    const bgfx::Memory* mem = bgfx::alloc(headerLen + metadataLen + shaderSizeFieldLen + sourceLen);

    uint8_t* data = mem->data;
    memcpy(data, isVertex ? "VSH" : "FSH", 3);
    data[3] = 0x05; // Version 5
    memset(data + 4, 0, 4); // Hash

    // Uniform Count (offset 8, 2 bytes, little-endian)
    data[8] = (uint8_t)(uniformCount & 0xFF);
    data[9] = (uint8_t)((uniformCount >> 8) & 0xFF);

    uint8_t* curr = data + 10;
    if (!isVertex && uniformCount == 1) {
        const char* name = "ourTexture";
        uint8_t nameLen = (uint8_t)strlen(name);
        *curr++ = nameLen;
        memcpy(curr, name, nameLen);
        curr += nameLen;
        *curr++ = (uint8_t)bgfx::UniformType::Sampler;
        *curr++ = 1; // num (array size)

        // regIndex (2 bytes, little-endian)
        *curr++ = 0;
        *curr++ = 0;
        // regCount (2 bytes, little-endian)
        *curr++ = 1; // 1 for sampler
        *curr++ = 0;
    }

    // Shader size field (4 bytes, little-endian)
    *curr++ = (uint8_t)(sourceLen & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 8) & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 16) & 0xFF);
    *curr++ = (uint8_t)((sourceLen >> 24) & 0xFF);

    memcpy(curr, source, sourceLen);

    return mem;
}

SplashRenderer::SplashRenderer()
        : shaderProgram(BGFX_INVALID_HANDLE)
        , vbh(BGFX_INVALID_HANDLE)
        , m_ibh(BGFX_INVALID_HANDLE)
        , s_texColor(BGFX_INVALID_HANDLE)
        , isValid(false) {
}

SplashRenderer::~SplashRenderer() {
    Shutdown();
}

void SplashRenderer::CreateShaderProgram() {
    const bgfx::Memory* vshMem = createShaderMem(vertexShaderSource, true, 0);
    bgfx::ShaderHandle vsh = bgfx::createShader(vshMem);
    if (!bgfx::isValid(vsh)) {
        LOGE("Failed to create vertex shader");
        return;
    }

    const bgfx::Memory* fshMem = createShaderMem(fragmentShaderSource, false, 1);
    bgfx::ShaderHandle fsh = bgfx::createShader(fshMem);
    if (!bgfx::isValid(fsh)) {
        LOGE("Failed to create fragment shader");
        bgfx::destroy(vsh);
        return;
    }

    shaderProgram = bgfx::createProgram(vsh, fsh, true);
    if (!bgfx::isValid(shaderProgram)) {
        LOGE("Failed to create shader program");
    } else {
        LOGI("Shader program created successfully");
    }
}

void SplashRenderer::Init() {
    Shutdown(); // Ensure clean state

    PosTexCoordVertex::init();
    CreateShaderProgram();
    if (!bgfx::isValid(shaderProgram)) return;

    static const char* kTexUniform = "ourTexture";
    LOGI("Creating uniform: %s", kTexUniform);
    s_texColor = bgfx::createUniform(kTexUniform, bgfx::UniformType::Sampler);

    PosTexCoordVertex vertices[] = {
            {-1.0f,  1.0f,  0.0f, 0.0f}, // 0: Top-Left
            { 1.0f,  1.0f,  1.0f, 0.0f}, // 1: Top-Right
            {-1.0f, -1.0f,  0.0f, 1.0f}, // 2: Bottom-Left
            { 1.0f, -1.0f,  1.0f, 1.0f}, // 3: Bottom-Right
    };

    vbh = bgfx::createVertexBuffer(bgfx::copy(vertices, sizeof(vertices)), PosTexCoordVertex::ms_layout);

    uint16_t indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    m_ibh = bgfx::createIndexBuffer(bgfx::copy(indices, sizeof(indices)));

    isValid = bgfx::isValid(vbh) && bgfx::isValid(m_ibh);
    LOGI("SplashRenderer::Init completed. isValid: %d, vbh: %hu", isValid, vbh.idx);
}

void SplashRenderer::Shutdown() {
    if (bgfx::isValid(shaderProgram)) {
        bgfx::destroy(shaderProgram);
        shaderProgram = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(vbh)) {
        bgfx::destroy(vbh);
        vbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_ibh)) {
        bgfx::destroy(m_ibh);
        m_ibh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(s_texColor)) {
        bgfx::destroy(s_texColor);
        s_texColor = BGFX_INVALID_HANDLE;
    }
    isValid = false;
}

void SplashRenderer::Render(SplashTexture& tex, bgfx::ViewId viewId) {
    if (!isValid || !bgfx::isValid(tex.handle)) {
        LOGE("SplashRenderer::Render skipping: isValid=%d, tex.handle=%hu", isValid, tex.handle.idx);
        return;
    }

    if (PosTexCoordVertex::ms_layout.m_hash == 0) {
        LOGE("VertexLayout hash is 0 in Render! Stride: %d. Re-initializing...", PosTexCoordVertex::ms_layout.m_stride);
        PosTexCoordVertex::init();
        if (PosTexCoordVertex::ms_layout.m_hash == 0) {
            LOGE("Failed to re-initialize VertexLayout hash!");
            return;
        }
    }

    // View rect should be handled by the caller (Game::UpdateGame3D)
    // but we set up the transform for a full-screen quad within that view
    float view[16];
    float proj[16];
    bx::mtxIdentity(view);
    bx::mtxOrtho(proj, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(viewId, view, proj);

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::setTexture(0, s_texColor, tex.handle);

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
    bgfx::submit(viewId, shaderProgram);
}
