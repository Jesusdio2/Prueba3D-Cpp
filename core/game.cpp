#include "game.h"
#include "scene.h"
#include "renderer/renderer.h"
#include "platform/opengl/opengl_renderer.h"
#include "splash/splash_texture.h"
#include "splash/splash_renderer.h"
#include <iostream>

static Scene* mainScene = nullptr;
static Renderer* renderer = nullptr;

static GameState state = GameState::SPLASH;
static float splashTime = 0.0f;

static SplashTexture splashTex;
static SplashRenderer splashRenderer;

extern "C" {

EXPORT_API void InitGame3D() {
    std::cout << "Inicializando juego 3D..." << std::endl;

    renderer = new OpenGLRenderer();
    mainScene = new Scene();

    renderer->Init();

    splashTex.Load("assets/splash_screen.png");
    splashRenderer.Init();

    mainScene->LoadTestScene();
}

EXPORT_API void UpdateGame3D(float deltaTime) {

    splashTime += deltaTime;

    if (state == GameState::SPLASH) {

        if (splashTime < 2.5f) {
            renderer->BeginFrame();
            splashRenderer.Render(splashTex);
            renderer->EndFrame();
        } else {
            state = GameState::IN_GAME;
        }

    } else if (state == GameState::IN_GAME) {

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

} // 👈 ESTE CIERRE FALTABA