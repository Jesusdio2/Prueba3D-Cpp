#include "game.h"
#include "scene.h"
#include "renderer/renderer.h"
#include "platform/opengl/opengl_renderer.h"
#include <iostream>

static Scene* mainScene = nullptr;
static Renderer* renderer = nullptr;

extern "C" {

EXPORT_API void InitGame3D() {
    std::cout << "Inicializando juego 3D..." << std::endl;

    renderer = new OpenGLRenderer(); // ✔ polimorfismo correcto
    mainScene = new Scene();

    renderer->Init();
    mainScene->LoadTestScene();
}

EXPORT_API void UpdateGame3D(float deltaTime) {
    if (renderer && mainScene) {
        mainScene->Update(deltaTime);

        renderer->BeginFrame();
        renderer->Draw();
        renderer->EndFrame();
    }
}

EXPORT_API void ShutdownGame3D() {
    std::cout << "Cerrando juego 3D..." << std::endl;

    delete mainScene;
    delete renderer;

    mainScene = nullptr;
    renderer = nullptr;
}

} // extern "C"