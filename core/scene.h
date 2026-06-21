// scene.h
#pragma once

#include <bgfx/bgfx.h>

class Scene {
public:
    Scene();
    ~Scene();

    void LoadTestScene();
    void Update(float deltaTime);
    void Render(int width, int height);

private:
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;
    float m_time;
};
