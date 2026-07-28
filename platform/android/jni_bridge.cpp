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
Java_com_faes_prueba3d_NativeBridge_updateViewport(JNIEnv*, jobject, jint width, jint height) {
    UpdateViewport(width, height);
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
    InputEvent event;
    event.device = InputDeviceType::Touch;
    event.action = (action == 0) ? InputAction::Down : (action == 1 ? InputAction::Up : InputAction::Move);
    event.x = x;
    event.y = y;
    event.keyCode = 0;
    PushInputEvent(event);
}

JNIEXPORT jboolean JNICALL
Java_com_faes_prueba3d_NativeBridge_onKeyEvent(JNIEnv*, jobject, jint keyCode, jboolean isPressed) {
    InputEvent event;
    event.device = InputDeviceType::Keyboard; // O Gamepad, según origen
    event.action = isPressed ? InputAction::Down : InputAction::Up;
    event.keyCode = keyCode;
    event.x = 0; event.y = 0;
    PushInputEvent(event);
    return true;
}

JNIEXPORT jboolean JNICALL
Java_com_faes_prueba3d_NativeBridge_onGenericMotionEvent(JNIEnv* env, jobject, jobject eventObj) {
    // Aquí se extraerían los ejes AXIS_X/Y y se enviaría como InputAction::Axis
    return true;
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_onInputEvent(JNIEnv*, jobject, jint device, jint action, jint keyCode, jfloat x, jfloat y, jfloat ax, jfloat ay) {
    InputEvent event;
    event.device = static_cast<InputDeviceType>(device);
    event.action = static_cast<InputAction>(action);
    event.keyCode = keyCode;
    event.x = x;
    event.y = y;
    event.axisX = ax;
    event.axisY = ay;
    PushInputEvent(event);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_setDeviceType(JNIEnv*, jobject, jint type, jboolean isEmulator) {
    SetDeviceType(type, isEmulator);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_onPause(JNIEnv*, jobject) {
    // Pausar motor o audio
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_NativeBridge_onResume(JNIEnv*, jobject) {
    // Reanudar
}

JNIEXPORT jboolean JNICALL
Java_com_faes_prueba3d_NativeBridge_shouldQuit(JNIEnv*, jobject) {
    return ShouldQuit();
}

}
