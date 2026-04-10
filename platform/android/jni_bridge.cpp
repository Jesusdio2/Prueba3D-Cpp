#include <jni.h>
#include "../../core/game.h"

extern "C" {

// 👇 IMPORTANTE: coincide con GameSurface.NativeRenderer
JNIEXPORT void JNICALL
Java_com_faes_prueba3d_GameSurface_00024NativeRenderer_initGame(JNIEnv*, jobject) {
    InitGame3D();
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_GameSurface_00024NativeRenderer_updateGame(JNIEnv*, jobject, jfloat delta) {
    UpdateGame3D(delta);
}

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_GameSurface_00024NativeRenderer_shutdownGame(JNIEnv*, jobject) {
    ShutdownGame3D();
}

}