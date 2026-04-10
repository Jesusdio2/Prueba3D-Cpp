// core/game.h
#pragma once

#ifdef _WIN32
    #define EXPORT_API __declspec(dllexport)
#else
    #define EXPORT_API
#endif

extern "C" {
    EXPORT_API void InitGame3D();
    EXPORT_API void UpdateGame3D(float deltaTime);
    EXPORT_API void ShutdownGame3D();
}