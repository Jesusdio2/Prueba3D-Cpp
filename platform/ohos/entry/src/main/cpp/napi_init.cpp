#include "napi/native_api.h"
#include "ace/xcomponent/native_interface_xcomponent.h"
#include "native_window/external_window.h"
#include "hilog/log.h"
#include <thread>
#include <chrono>

// Variables globales para la gestión del ciclo de renderizado nativo
static OH_NativeXComponent* g_xcomponent = nullptr;
static OHNativeWindow* g_nativeWindow = nullptr;
static uint64_t g_width = 0;
static uint64_t g_height = 0;
static bool g_running = false;
static std::thread g_renderThread;

// ---- FUNCIONES DEL GAME LOOP NATIVO EN C++ ----

void GameRenderLoop() {
    // ---- 🔥 AQUÍ INICIALIZAS TU ENGINE CON BGFX ----
    // Ya tienes acceso directo a:
    // g_nativeWindow -> Puntero de ventana nativa de Huawei
    // g_width y g_height -> Dimensiones de la pantalla
    
    // Ejemplo ficticio de llamada a tu core:
    // NativeBridge::initGame(g_nativeWindow, g_width, g_height);
    
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (g_running) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - lastTime;
        lastTime = now;
        float deltaTime = elapsed.count();

        // 1. Actualizar lógica e interacciones del motor gráfico
        // NativeBridge::updateGame(deltaTime);

        // Controlar la tasa de refresco del hilo para no saturar la CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // 2. Liberar recursos de BGFX y BX al detener la ejecución
    // NativeBridge::shutdownGame();
}

// ---- IMPLEMENTACIÓN DE MÉTODOS DEL NATIVE BRIDGE ----

static napi_value InitGameLoop(napi_env env, napi_callback_info info) {
    if (!g_running) {
        g_running = true;
        g_renderThread = std::thread(GameRenderLoop);
        g_renderThread.detach();
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "NativeBridge", "Hilo de renderizado continuo arrancado exitosamente.");
    }
    return nullptr;
}

static napi_value ShutdownGame(napi_env env, napi_callback_info info) {
    g_running = false;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "NativeBridge", "Señal de apagado enviada al juego.");
    return nullptr;
}

static napi_value SetGameState(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int32_t state = 0;
    napi_get_value_int32(env, args[0], &state);
    
    // NativeBridge::setGameState(state);
    return nullptr;
}

static napi_value OnTouch(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double x = 0, y = 0;
    int32_t action = 0;
    napi_get_value_double(env, args[0], &x);
    napi_get_value_double(env, args[1], &y);
    napi_get_value_int32(env, args[2], &action);

    // NativeBridge::onTouch(static_cast<float>(x), static_cast<float>(y), action);
    return nullptr;
}

static napi_value OnKeyEvent(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    int32_t keyCode = 0;
    bool isPressed = false;
    napi_get_value_int32(env, args[0], &keyCode);
    napi_get_value_bool(env, args[1], &isPressed);

    // bool consumed = NativeBridge::onKeyEvent(keyCode, isPressed);
    bool consumed = true;

    napi_value result;
    napi_get_boolean(env, consumed, &result);
    return result;
}

static napi_value ShouldQuit(napi_env env, napi_callback_info info) {
    // bool quit = NativeBridge::shouldQuit();
    bool quit = false;
    
    napi_value result;
    napi_get_boolean(env, quit, &result);
    return result;
}

// ---- REGISTRO DE CAPTURA DEL SURFACE (XCOMPONENT CALLBACKS) ----

static void OnSurfaceCreated(OH_NativeXComponent* component, void* window) {
    g_xcomponent = component;
    g_nativeWindow = static_cast<OHNativeWindow*>(window);
    
    // Extraer las dimensiones físicas del viewport de visualización
    OH_NativeXComponent_GetXComponentSize(component, window, &g_width, &g_height);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "NativeBridge", "Surface Creado. Ancho: %lld, Alto: %lld", g_width, g_height);
}

static void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window) {
    g_running = false;
    g_nativeWindow = nullptr;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "NativeBridge", "Surface destruido por el ciclo del sistema.");
}

static OH_NativeXComponent_Callback g_xcomponentCallbacks = {
    .OnSurfaceCreated = OnSurfaceCreated,
    .OnSurfaceChanged = nullptr,
    .OnSurfaceDestroyed = OnSurfaceDestroyed,
    .DispatchTouchEvent = nullptr
};

// ---- INICIALIZACIÓN MÓDULO PRINCIPAL NAPI ----

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    // 1. Mapear las funciones expuestas hacia TypeScript
    napi_property_descriptor desc[] = {
        { "initGameLoop", nullptr, InitGameLoop, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "shutdownGame", nullptr, ShutdownGame, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setGameState", nullptr, SetGameState, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "onTouch", nullptr, OnTouch, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "onKeyEvent", nullptr, OnKeyEvent, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "shouldQuit", nullptr, ShouldQuit, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    // 2. Capturar de forma automática la instancia de la vista XComponent
    napi_value exportInstance = nullptr;
    napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    OH_NativeXComponent* nativeXComponent = nullptr;
    napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent));
    
    if (nativeXComponent) {
        OH_NativeXComponent_RegisterCallback(nativeXComponent, &g_xcomponentCallbacks);
    }

    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
    napi_module_register(&demoModule);
}
