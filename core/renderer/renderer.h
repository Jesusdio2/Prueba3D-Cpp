// core/renderer/renderer.h
#pragma once

class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void Init() = 0;
    virtual void BeginFrame() = 0;
    virtual void Draw() = 0;
    virtual void EndFrame() = 0;
};