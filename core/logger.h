#pragma once

#ifdef __ANDROID__
#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Prueba3D", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Prueba3D", __VA_ARGS__)
#else
#include <cstdio>
#define LOGE(...) { printf("[ERROR] "); printf(__VA_ARGS__); printf("\n"); }
#define LOGI(...) { printf("[INFO] "); printf(__VA_ARGS__); printf("\n"); }
#endif
