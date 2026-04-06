#include <jni.h>
#include "../../core/game.h"

extern "C" {

JNIEXPORT void JNICALL
Java_com_faes_prueba3d_GameActivity_initGame(JNIEnv *env, jobject thiz) {
    // Llamamos a la función de tu motor en /core/
    InitGame3D();
}

}
