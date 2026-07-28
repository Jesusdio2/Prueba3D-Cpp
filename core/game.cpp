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
#include <queue>
#include <mutex>

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

static std::queue<InputEvent> gInputQueue;
static std::mutex gInputMutex;

static SplashTexture splashTex;
static SplashTexture buttonTex;
static SplashRenderer splashRenderer;
static TextRenderer textRenderer;

static int screenWidth = 1920;
static int screenHeight = 1080;
static float gScale = 1.0f;
static float gOffsetX = 0.0f;
static float gOffsetY = 0.0f;

// Helper para mapeo de coordenadas sin allocs en el frame
struct ViewRect { uint16_t x, y, w, h; };
static ViewRect getPhysicalRect(int lx, int ly, int lw, int lh) {
    return {
        (uint16_t)(lx * gScale + gOffsetX),
        (uint16_t)(ly * gScale + gOffsetY),
        (uint16_t)(lw * gScale),
        (uint16_t)(lh * gScale)
    };
}

extern "C" {

EXPORT_API void PushInputEvent(const InputEvent& event) {
    std::lock_guard<std::mutex> lock(gInputMutex);
    gInputQueue.push(event);
}

static void ProcessInput() {
    std::lock_guard<std::mutex> lock(gInputMutex);
    while (!gInputQueue.empty()) {
        InputEvent event = gInputQueue.front();
        gInputQueue.pop();

        if (event.action == InputAction::Down) {
            if (state == GameState::MENU) {
                float lx = (event.x - gOffsetX) / gScale;
                float ly = (event.y - gOffsetY) / gScale;

                if (lx > 800 && lx < 1120) {
                    if (ly > 400 && ly < 480) state = GameState::IN_GAME;
                    else if (ly > 600 && ly < 680) shouldQuit = true;
                }

                // Manejo de teclado/gamepad (Enter o botón central)
                if (event.keyCode == 66 || event.keyCode == 23) {
                     state = GameState::IN_GAME;
                }
            }
            else if (state == GameState::IN_GAME) {
                if (event.keyCode == 4) { // Back
                     state = GameState::MENU;
                } else {
                     state = GameState::MENU;
                }
            }
        }
    }
}

EXPORT_API void UpdateViewport(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    float targetAspect = 1920.0f / 1080.0f;
    float currentAspect = (float)width / (float)height;

    if (currentAspect > targetAspect) {
        gScale = (float)height / 1080.0f;
        gOffsetX = (width - (1920.0f * gScale)) * 0.5f;
        gOffsetY = 0.0f;
    } else {
        gScale = (float)width / 1920.0f;
        gOffsetX = 0.0f;
        gOffsetY = (height - (1080.0f * gScale)) * 0.5f;
    }

    // CRÍTICO: Reajustar buffers internos de bgfx y Z-Buffer
    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
}

EXPORT_API void InitGame3D(void* windowHandle, void* assetManager, int width, int height) {
    LOGI("Inicializando juego 3D con bgfx. Res: %dx%d", width, height);
    gAssetManager = (AAssetManager*)assetManager;
    UpdateViewport(width, height);

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

EXPORT_API void SetDeviceType(int type, bool isEmulator) {
    LOGI("Dispositivo detectado: Tipo %d, Emulador: %s", type, isEmulator ? "SI" : "NO");
}

EXPORT_API void UpdateGame3D(float deltaTime) {
    ProcessInput();
    splashTime += deltaTime;

    // View 0 siempre limpia toda la pantalla física
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
    bgfx::touch(0);

    if (state == GameState::SPLASH) {
        // Centrar Splash en 16:9
        ViewRect r = getPhysicalRect(0, 0, 1920, 1080);
        bgfx::setViewRect(10, r.x, r.y, r.w, r.h);
        bgfx::setViewClear(10, BGFX_CLEAR_NONE);
        splashRenderer.Render(splashTex, 10);

        if (splashTime >= 2.5f) {
            state = GameState::MENU;
        }
    }
    else if (state == GameState::MENU) {
        // Renderizado de Fondo del Menú (View 11) centrado
        ViewRect rBg = getPhysicalRect(0, 0, 1920, 1080);
        bgfx::setViewRect(11, rBg.x, rBg.y, rBg.w, rBg.h);
        bgfx::setViewClear(11, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::touch(11);

        // Botón JUGAR (View 12)
        ViewRect rBtn = getPhysicalRect(800, 400, 320, 120);
        bgfx::setViewRect(12, rBtn.x, rBtn.y, rBtn.w, rBtn.h);
        bgfx::setViewClear(12, BGFX_CLEAR_NONE);
        splashRenderer.Render(buttonTex, 12);

        // Texto e Interfaz (View 255) centrado en el área 16:9
        bgfx::setViewRect(255, rBg.x, rBg.y, rBg.w, rBg.h);
        bgfx::setViewClear(255, BGFX_CLEAR_NONE);
        textRenderer.RenderText("JUGAR", 880, 480, 0xffffffff, 255);
        textRenderer.RenderText("PULSA [ENTER] O TOCA", 780, 800, 0xaaaaaaff, 255);
    }
    else if (state == GameState::IN_GAME) {
        // El juego 3D sí ocupa toda la pantalla (Aspect Ratio variable)
        mainScene->Update(deltaTime);
        bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
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
            float lx = (x - gOffsetX) / gScale;
            float ly = (y - gOffsetY) / gScale;

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
