// core/platform/opengl/opengl_renderer.cpp
#include "opengl_renderer.h"

#ifdef ANDROID_PLATFORM
#include <GLES3/gl3.h>
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Renderer", __VA_ARGS__)
#else
#include <GL/gl.h>
#include <iostream>
#define LOGI(...) std::cout << __VA_ARGS__ << std::endl
#endif

OpenGLRenderer::OpenGLRenderer() {}
OpenGLRenderer::~OpenGLRenderer() {}

void OpenGLRenderer::Init() {
    LOGI("OpenGLRenderer Init");

    // Color inicial
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OpenGLRenderer::BeginFrame() {
    // Limpia pantalla con negro (o el color actual)
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::Draw() {
    // 🔥 TEST VISUAL: cambia el fondo a rojo dinámicamente
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void OpenGLRenderer::EndFrame() {
    // En Android GLSurfaceView NO necesitas swap manual
}