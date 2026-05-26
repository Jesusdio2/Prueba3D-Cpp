#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <swappy/swappyGL.h>
#include "../../core/game.h"

extern "C" {

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_initGame(JNIEnv* env, jobject, jobject activity, jobject surface, jobject assetManager, jint width, jint height) {
    ANativeWindow* window = nullptr;
    if (surface != nullptr) {
        window = ANativeWindow_fromSurface(env, surface);
    }

    AAssetManager* am = nullptr;
    if (assetManager != nullptr) {
        am = AAssetManager_fromJava(env, assetManager);
    }

    // Inicializar Swappy
    // SwappyGL_init detecta el refresco de pantalla y configura la cadencia de frames.
    SwappyGL_init(env, activity);
    if (window) {
        SwappyGL_setWindow(window);
    }

    InitGame3D(window, am, width, height);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_updateGame(JNIEnv*, jobject, jfloat delta) {
    // Para OpenGL, Swappy normalmente requiere reemplazar eglSwapBuffers con SwappyGL_swap.
    // Dado que bgfx maneja el swap internamente, la inicialización previa ya ayuda
    // a que el sistema operativo ajuste la tasa de refresco (vía ANativeWindow_setFrameRate).
    UpdateGame3D(delta);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_shutdownGame(JNIEnv*, jobject) {
    ShutdownGame3D();
    SwappyGL_destroy();
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_setGameState(JNIEnv*, jobject, jint state) {
    SetGameState(state);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_onTouch(JNIEnv*, jobject, jfloat x, jfloat y, jint action) {
    OnTouch(x, y, action);
}

JNIEXPORT jboolean JNICALL
Java_com_faes_prueba3d_NativeBridge_shouldQuit(JNIEnv*, jobject) {
    return ShouldQuit();
}

}
