#include "renderer.h"
#include <iostream>

Renderer::Renderer() {}
Renderer::~Renderer() {}

void Renderer::Init() {
    std::cout << "Renderer inicializado (placeholder)" << std::endl;
}

void Renderer::RenderScene(Scene* scene) {
    std::cout << "Renderizando escena..." << std::endl;
}