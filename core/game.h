// core/game.h
#pragma once

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

enum class GameState {
    SPLASH,
    MENU,
    IN_GAME
};

extern "C" {
EXPORT_API void InitGame3D(void* windowHandle = nullptr, void* assetManager = nullptr, int width = 1920, int height = 1080);
EXPORT_API void UpdateGame3D(float deltaTime);
EXPORT_API void ShutdownGame3D();
EXPORT_API void SetGameState(int state);
EXPORT_API void OnTouch(float x, float y, int action);
EXPORT_API bool ShouldQuit();
}