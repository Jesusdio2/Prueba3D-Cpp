// scene.cpp
#include "scene.h"
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <string.h>

struct PosColorVertex {
    float x, y, z;
    uint32_t abgr;

    static bgfx::VertexLayout ms_layout;
    static void init() {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static const PosColorVertex s_cubeVertices[] = {
    {-1.0f,  1.0f,  1.0f, 0xff0000ff },
    { 1.0f,  1.0f,  1.0f, 0xff00ff00 },
    {-1.0f, -1.0f,  1.0f, 0xffff0000 },
    { 1.0f, -1.0f,  1.0f, 0xffffffff },
    {-1.0f,  1.0f, -1.0f, 0xff0000ff },
    { 1.0f,  1.0f, -1.0f, 0xff00ff00 },
    {-1.0f, -1.0f, -1.0f, 0xffff0000 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeIndices[] = {
    0, 1, 2, 1, 3, 2,
    4, 6, 5, 5, 6, 7,
    0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3,
    0, 4, 1, 4, 5, 1,
    2, 3, 6, 6, 3, 7,
};

// Shaders manuales para OpenGL ES 3.0 (similar a SplashRenderer)
static const char* cubeVSH =
    "#version 300 es\n"
    "layout(location=0) in vec3 a_position;\n"
    "layout(location=1) in vec4 a_color0;\n"
    "uniform mat4 u_modelViewProj;\n"
    "out vec4 v_color0;\n"
    "void main() {\n"
    "    gl_Position = u_modelViewProj * vec4(a_position, 1.0);\n"
    "    v_color0 = a_color0;\n"
    "}\n";

static const char* cubeFSH =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 v_color0;\n"
    "out vec4 o_color;\n"
    "void main() {\n"
    "    o_color = v_color0;\n"
    "}\n";

// Helper para crear el buffer de memoria que bgfx espera para un shader "raw"
static const bgfx::Memory* createShaderMem(const char* source, bool isVertex, int uniformCount = 0) {
    uint32_t len = (uint32_t)strlen(source);
    // Header (10 bytes) + Metadata (si hay uniforms) + size (4 bytes) + source
    uint32_t metadataLen = 0;
    if (uniformCount > 0) {
        // "u_modelViewProj" -> len(1) + name(15) + type(1) + num(1) + regIndex(2) + regCount(2) = 22
        metadataLen = 1 + 15 + 1 + 1 + 2 + 2;
    }
    const bgfx::Memory* mem = bgfx::alloc(10 + metadataLen + 4 + len);
    uint8_t* data = mem->data;
    memcpy(data, isVertex ? "VSH" : "FSH", 3);
    data[3] = 0x05; // Version
    memset(data + 4, 0, 4); // Hash
    data[8] = (uint8_t)uniformCount; data[9] = 0; // Uniform count

    uint8_t* curr = data + 10;
    if (uniformCount > 0) {
        const char* name = "u_modelViewProj";
        *curr++ = (uint8_t)strlen(name);
        memcpy(curr, name, strlen(name)); curr += strlen(name);
        *curr++ = (uint8_t)bgfx::UniformType::Mat4;
        *curr++ = 1; // num
        *curr++ = 0; *curr++ = 0; // regIndex
        *curr++ = 4; *curr++ = 0; // regCount (4 registers for mat4)
    }

    uint32_t* sizeField = (uint32_t*)curr;
    *sizeField = len;
    memcpy(curr + 4, source, len);
    return mem;
}

Scene::Scene()
    : m_vbh(BGFX_INVALID_HANDLE)
    , m_ibh(BGFX_INVALID_HANDLE)
    , m_program(BGFX_INVALID_HANDLE)
    , m_time(0.0f) {
}

Scene::~Scene() {
    if (bgfx::isValid(m_program)) bgfx::destroy(m_program);
    if (bgfx::isValid(m_vbh)) bgfx::destroy(m_vbh);
    if (bgfx::isValid(m_ibh)) bgfx::destroy(m_ibh);
}

void Scene::LoadTestScene() {
    PosColorVertex::init();

    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices)));

    bgfx::ShaderHandle vsh = bgfx::createShader(createShaderMem(cubeVSH, true, 1));
    bgfx::ShaderHandle fsh = bgfx::createShader(createShaderMem(cubeFSH, false, 0));
    m_program = bgfx::createProgram(vsh, fsh, true);
}

void Scene::Update(float deltaTime) {
    m_time += deltaTime;
}

void Scene::Render(int width, int height) {
    float view[16];
    float proj[16];

    bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
    bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
    bx::mtxLookAt(view, eye, at);
    bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);
    bgfx::setViewRect(0, 0, 0, (uint16_t)width, (uint16_t)height);

    float mtx[16];
    bx::mtxRotateXY(mtx, m_time, m_time * 0.37f);
    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);

    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, m_program);
}
