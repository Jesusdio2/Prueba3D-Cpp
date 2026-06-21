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
Java_com_faes_prueba3d_NativeBridge_onKeyEvent(JNIEnv*, jobject, jint keyCode, jboolean isPressed) {
    // 19 = DPAD_UP, 20 = DPAD_DOWN, 21 = DPAD_LEFT, 22 = DPAD_RIGHT, 23 = DPAD_CENTER, 66 = ENTER, 4 = BACK
    // En un juego real, pasaríamos el código a C++ para manejarlo allí
    if (isPressed) {
        if (keyCode == 23 || keyCode == 66) { // Center o Enter
            OnTouch(0, 0, 0); // Simular un touch para el botón JUGAR en el menú
            return true;
        }
        if (keyCode == 4) { // Back
            SetGameState(1); // Volver al menú (MENU = 1)
            return true;
        }
    }
    return false;
}

JNIEXPORT jboolean JNICALL
Java_com_faes_prueba3d_NativeBridge_shouldQuit(JNIEnv*, jobject) {
    return ShouldQuit();
}

}
