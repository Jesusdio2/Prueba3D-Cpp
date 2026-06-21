#include "game.h"
#include "scene.h"
#include "splash/splash_texture.h"
#include "splash/splash_renderer.h"
#include "renderer/text_renderer.h"
#include "logger.h"
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <vector>

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
typedef AAssetManager AssetManager;
#else
typedef void AssetManager;
#endif

static Scene* mainScene = nullptr;
static AssetManager* gAssetManager = nullptr;
static GameState state = GameState::SPLASH;
static float splashTime = 0.0f;
static bool shouldQuit = false;

static SplashTexture splashTex;
static SplashTexture buttonTex;
static SplashRenderer splashRenderer;
static TextRenderer textRenderer;

static int screenWidth = 1920;
static int screenHeight = 1080;

// Helper para mapeo de coordenadas sin allocs en el frame
struct ViewRect { uint16_t x, y, w, h; };
static ViewRect getPhysicalRect(int lx, int ly, int lw, int lh) {
    return {
        (uint16_t)(lx * screenWidth / 1920),
        (uint16_t)(ly * screenHeight / 1080),
        (uint16_t)(lw * screenWidth / 1920),
        (uint16_t)(lh * screenHeight / 1080)
    };
}

extern "C" {

EXPORT_API void InitGame3D(void* windowHandle, void* assetManager, int width, int height) {
    LOGI("Inicializando juego 3D con bgfx. Res: %dx%d", width, height);
    gAssetManager = (AAssetManager*)assetManager;
    screenWidth = width;
    screenHeight = height;

    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // Auto-detectar (GLES en Android)
    init.resolution.width = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.nwh = windowHandle;

    if (!bgfx::init(init)) {
        LOGE("Error al inicializar bgfx");
        return;
    }

    // Configuración base de todas las vistas usadas (0-4)
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    for (uint8_t i = 1; i < 5; ++i) {
        bgfx::setViewClear(i, BGFX_CLEAR_NONE, 0x000000ff, 1.0f, 0);
        bgfx::setViewMode(i, bgfx::ViewMode::Sequential);
    }
    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
    bgfx::setViewRect(0, 0, 0, (uint16_t)width, (uint16_t)height);

    mainScene = new Scene();
    splashTex.Load("splash_screen.png", gAssetManager);
    buttonTex.Load("Button.png", gAssetManager);
    splashRenderer.Init();
    textRenderer.Init("fonts/NotoSans-VariableFont_wdth,wght.ttf", 48.0f, gAssetManager);
    mainScene->LoadTestScene();
}

EXPORT_API void UpdateGame3D(float deltaTime) {
    splashTime += deltaTime;

    if (state == GameState::SPLASH) {
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
        splashRenderer.Render(splashTex, 0);

        if (splashTime >= 2.5f) {
            state = GameState::MENU;
        }
    }
    else if (state == GameState::MENU) {
        // Clear de la pantalla de fondo (View 0)
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
        bgfx::touch(0);

        // Renderizado de Texto e Interfaz (View 255)
        bgfx::setViewMode(255, bgfx::ViewMode::Sequential);
        bgfx::setViewClear(255, BGFX_CLEAR_NONE);
        bgfx::setViewRect(255, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);

        // Botón JUGAR (View 1)
        ViewRect r1 = getPhysicalRect(800, 400, 320, 120);
        bgfx::setViewRect(1, r1.x, r1.y, r1.w, r1.h);
        bgfx::setViewClear(1, BGFX_CLEAR_NONE);
        splashRenderer.Render(buttonTex, 1);

        // Texto encima (View 255)
        textRenderer.RenderText("JUGAR", 880, 480, 0xffffffff, 255);
        textRenderer.RenderText("PULSA [ENTER] O TOCA", 780, 800, 0xaaaaaaff, 255);
    }
    else if (state == GameState::IN_GAME) {
        mainScene->Update(deltaTime);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
        bgfx::touch(0);
        mainScene->Render(screenWidth, screenHeight);
    }

    bgfx::frame();
}

EXPORT_API void ShutdownGame3D() {
    LOGI("Cerrando juego 3D...");
    delete mainScene;
    splashRenderer.Shutdown();
    textRenderer.Shutdown();
    splashTex.Destroy();
    buttonTex.Destroy();
    bgfx::shutdown();
    mainScene = nullptr;
}

EXPORT_API void SetGameState(int state) {
    ::state = static_cast<GameState>(state);
}

EXPORT_API void OnTouch(float x, float y, int action) {
    if (action == 0) { // ACTION_DOWN
        if (state == GameState::MENU) {
            float lx = x * 1920.0f / (float)screenWidth;
            float ly = y * 1080.0f / (float)screenHeight;

            if (lx > 800 && lx < 1120) {
                if (ly > 400 && ly < 480) state = GameState::IN_GAME;
                else if (ly > 600 && ly < 680) shouldQuit = true;
            }
        }
        else if (state == GameState::IN_GAME) {
            state = GameState::MENU;
        }
    }
}

EXPORT_API bool ShouldQuit() {
    return shouldQuit;
}

} // extern "C"
