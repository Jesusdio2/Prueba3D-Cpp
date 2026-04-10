// core/platform/opengl/opengl_renderer.h
#pragma once

#include "../../renderer/renderer.h"

class OpenGLRenderer : public Renderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;

    void Init() override;
    void BeginFrame() override;
    void Draw() override;
    void EndFrame() override;
};