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
static bool gIsInitialized = false;

static std::queue<InputEvent> gInputQueue;
static std::mutex gInputMutex;

static SplashTexture splashTex;
static SplashTexture buttonTex;
static SplashRenderer splashRenderer;
static TextRenderer textRenderer;

struct FileLoggerCallback : public bgfx::CallbackI {
    void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override {
        FILE* f = fopen("Prueba3D.log", "a");
        if (f) {
            fprintf(f, "[BGFX FATAL] %s:%hu: (%d) %s\n", _filePath ? _filePath : "", _line, (int)_code, _str);
            fclose(f);
        }
    }

    void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override {
        char buf[2048];
        vsnprintf(buf, sizeof(buf), _format, _argList);
        FILE* f = fopen("Prueba3D.log", "a");
        if (f) {
            fprintf(f, "[BGFX TRACE] %s:%hu: %s\n", _filePath ? _filePath : "", _line, buf);
            fclose(f);
        }
    }

    void profilerBegin(const char*, uint32_t, const char*, uint16_t) override {}
    void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override {}
    void profilerEnd() override {}
    uint32_t cacheRead(uint64_t, void*, uint32_t) override { return 0; }
    void cacheWrite(uint64_t, const void*, uint32_t) override {}
    void screenShot(const char*, uint32_t, uint32_t, uint32_t, const void*, uint32_t, bool) override {}
    void captureBegin(uint32_t, uint32_t, uint32_t, uint32_t, bool) override {}
    void captureEnd() override {}
    void captureFrame(const void*, uint32_t) override {}
};

static FileLoggerCallback gBGFXLogger;

static bool gSplashTexOk = false;
static bool gButtonTexOk = false;
static bool gSplashRendererOk = false;
static bool gTextRendererOk = false;
static std::string gInitErrorMessage;

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

    // CRÍTICO: Reajustar buffers internos de bgfx solo si ya fue inicializado
    if (gIsInitialized && bgfx::getCaps() != nullptr) {
        bgfx::SwapChain swapChain;
        swapChain.width = (uint32_t)width;
        swapChain.height = (uint32_t)height;
        bgfx::reset(BGFX_RESET_VSYNC, &swapChain);
    }
}

EXPORT_API void InitGame3D(void* windowHandle, void* assetManager, int width, int height) {
    FILE* logF = fopen("Prueba3D.log", "w");
    if (logF) {
        fprintf(logF, "InitGame3D started. Res: %dx%d, windowHandle: %p\n", width, height, windowHandle);
        fflush(logF);
    }

    LOGI("Inicializando juego 3D con bgfx. Res: %dx%d", width, height);
    gAssetManager = (AssetManager*)assetManager;
    gInitErrorMessage.clear();
    UpdateViewport(width, height);

    bgfx::Init init;
#if defined(ANDROID) || defined(__ANDROID__)
    init.type = bgfx::RendererType::OpenGLES;
#elif defined(_WIN32) || defined(WINDOWS_PLATFORM)
    init.type = bgfx::RendererType::OpenGL;
#else
    init.type = bgfx::RendererType::Count; // Auto-detectar
#endif
    init.swapChain.width = (uint32_t)width;
    init.swapChain.height = (uint32_t)height;
    init.swapChain.nwh = windowHandle;
    init.swapChain.flags = BGFX_RESET_VSYNC;
    init.reset = BGFX_RESET_VSYNC;
    init.fallback = true;
    init.callback = &gBGFXLogger;

    if (logF) { fprintf(logF, "Calling bgfx::init...\n"); fflush(logF); }
    bool ok = bgfx::init(init);
    if (logF) { fprintf(logF, "bgfx::init result: %s\n", ok ? "TRUE" : "FALSE"); fflush(logF); }

    if (!ok) {
        LOGE("Error al inicializar bgfx");
        gInitErrorMessage = "bgfx::init() FAILED";
        if (logF) { fclose(logF); }
        return;
    }

    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Configuración base de todas las vistas usadas (0-4)
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1a1a2eff, 1.0f, 0);
    for (uint8_t i = 1; i < 5; ++i) {
        bgfx::setViewClear(i, BGFX_CLEAR_NONE, 0x000000ff, 1.0f, 0);
        bgfx::setViewMode(i, bgfx::ViewMode::Sequential);
    }
    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
    bgfx::setViewRect(0, 0, 0, (uint16_t)width, (uint16_t)height);

    if (logF) { fprintf(logF, "Loading Scene and Assets...\n"); fflush(logF); }
    mainScene = new Scene();
    gSplashTexOk = splashTex.Load("splash_screen.png", gAssetManager);
    if (!gSplashTexOk) { if (logF) fprintf(logF, "SplashTex load failed\n"); gInitErrorMessage += " [SplashTex fail]"; }

    gButtonTexOk = buttonTex.Load("Button.png", gAssetManager);
    if (!gButtonTexOk) { if (logF) fprintf(logF, "ButtonTex load failed\n"); gInitErrorMessage += " [ButtonTex fail]"; }

    splashRenderer.Init();
    gSplashRendererOk = splashRenderer.IsValid();
    if (!gSplashRendererOk) { if (logF) fprintf(logF, "SplashRenderer Init failed\n"); gInitErrorMessage += " [SplashRenderer Fail]"; }

    gTextRendererOk = textRenderer.Init("fonts/NotoSans-VariableFont_wdth,wght.ttf", 48.0f, gAssetManager);
    if (!gTextRendererOk) { if (logF) fprintf(logF, "TextRenderer Init failed\n"); gInitErrorMessage += " [TextRenderer Fail]"; }

    mainScene->LoadTestScene();
    gIsInitialized = true;
    if (logF) { fprintf(logF, "InitGame3D completed successfully. gIsInitialized = true\n"); fclose(logF); }
}

EXPORT_API void SetDeviceType(int type, bool isEmulator) {
    LOGI("Dispositivo detectado: Tipo %d, Emulador: %s", type, isEmulator ? "SI" : "NO");
}

EXPORT_API void UpdateGame3D(float deltaTime) {
    ProcessInput();
    splashTime += deltaTime;

    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::dbgTextClear();

    const char* rendererName = bgfx::getRendererName(bgfx::getCaps()->rendererType);
    bgfx::dbgTextPrintf(1, 1, 0x0f, "Prueba3D Status | Renderer: %s (%dx%d)", rendererName, screenWidth, screenHeight);
    bgfx::dbgTextPrintf(1, 2, gSplashTexOk ? 0x0a : 0x0c, "SplashTex: %s (idx: %d)", gSplashTexOk ? "OK" : "ERROR", splashTex.handle.idx);
    bgfx::dbgTextPrintf(1, 3, gButtonTexOk ? 0x0a : 0x0c, "ButtonTex: %s (idx: %d)", gButtonTexOk ? "OK" : "ERROR", buttonTex.handle.idx);
    bgfx::dbgTextPrintf(1, 4, gSplashRendererOk ? 0x0a : 0x0c, "SplashRenderer: %s", gSplashRendererOk ? "OK" : "ERROR");
    bgfx::dbgTextPrintf(1, 5, gTextRendererOk ? 0x0a : 0x0c, "TextRenderer: %s", gTextRendererOk ? "OK" : "ERROR");

    if (!gInitErrorMessage.empty()) {
        bgfx::dbgTextPrintf(1, 7, 0x4f, " DIAGNOSTIC ERROR: %s ", gInitErrorMessage.c_str());
    }

    // View 0 siempre limpia toda la pantalla física con color oscuro de fondo
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1a1a2eff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
    bgfx::touch(0);

    if (state == GameState::SPLASH) {
        ViewRect r = getPhysicalRect(0, 0, 1920, 1080);
        bgfx::setViewRect(10, r.x, r.y, r.w, r.h);
        bgfx::setViewClear(10, BGFX_CLEAR_NONE);
        if (gSplashRendererOk && gSplashTexOk) {
            splashRenderer.Render(splashTex, 10);
        }

        if (splashTime >= 2.5f) {
            state = GameState::MENU;
        }
    }
    else if (state == GameState::MENU) {
        ViewRect rBg = getPhysicalRect(0, 0, 1920, 1080);
        bgfx::setViewRect(11, rBg.x, rBg.y, rBg.w, rBg.h);
        bgfx::setViewClear(11, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::touch(11);

        ViewRect rBtn = getPhysicalRect(800, 400, 320, 120);
        bgfx::setViewRect(12, rBtn.x, rBtn.y, rBtn.w, rBtn.h);
        bgfx::setViewClear(12, BGFX_CLEAR_NONE);
        if (gSplashRendererOk && gButtonTexOk) {
            splashRenderer.Render(buttonTex, 12);
        }

        bgfx::setViewRect(255, rBg.x, rBg.y, rBg.w, rBg.h);
        bgfx::setViewClear(255, BGFX_CLEAR_NONE);
        if (gTextRendererOk) {
            textRenderer.RenderText("JUGAR", 880, 480, 0xffffffff, 255);
            textRenderer.RenderText("PULSA [ENTER] O TOCA", 780, 800, 0xaaaaaaff, 255);
        }
    }
    else if (state == GameState::IN_GAME) {
        if (mainScene) {
            mainScene->Update(deltaTime);
            bgfx::setViewRect(0, 0, 0, (uint16_t)screenWidth, (uint16_t)screenHeight);
            mainScene->Render(screenWidth, screenHeight);
        }
    }

    bgfx::frame();
}

EXPORT_API void ShutdownGame3D() {
    LOGI("Cerrando juego 3D...");
    gIsInitialized = false;
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

EXPORT_API bool IsGameInitialized() {
    return gIsInitialized;
}

} // extern "C"
