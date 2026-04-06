#pragma once

#include "scene.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init();
    void RenderScene(Scene* scene);
};