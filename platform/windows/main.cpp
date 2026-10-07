#include <windows.h>
#include <core/game.h>
#include <chrono>
#include <exception>
#include <cstdio>
#include <cstdarg>

static void TraceLog(const char* fmt, ...) {
    FILE* f = fopen("Prueba3D_Trace.log", "a");
    if (f) {
        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        fprintf(f, "\n");
        va_end(args);
        fclose(f);
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (uMsg) {
            case WM_CLOSE:
                DestroyWindow(hWnd);
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_ERASEBKGND:
                return 1; // Prevenir parpadeo y transparencia

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                if (!IsGameInitialized()) {
                    RECT rect;
                    GetClientRect(hWnd, &rect);
                    HBRUSH brush = CreateSolidBrush(RGB(26, 26, 46));
                    FillRect(hdc, &rect, brush);
                    DeleteObject(brush);

                    SetTextColor(hdc, RGB(255, 100, 100));
                    SetBkMode(hdc, TRANSPARENT);
                    const char* text = " ESTADO DE RENDERIZADO EN WINDOWS (main.cpp) \n\n"
                                       "[FALLO] El motor bgfx o la inicialización 3D no está listo.\n\n"
                                       "Diagnóstico:\n"
                                       "- Ventana Win32 HWND: Creada correctamente\n"
                                       "- IsGameInitialized(): FALSE\n"
                                       "- Backend gráfico: Falló la vinculación del contexto";
                    DrawTextA(hdc, text, -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
                }
                EndPaint(hWnd, &ps);
                return 0;
            }

            case WM_SIZE:
                if (wParam != SIZE_MINIMIZED && IsGameInitialized()) {
                    UpdateViewport(LOWORD(lParam), HIWORD(lParam));
                }
                return 0;

            case WM_LBUTTONDOWN:
                if (IsGameInitialized()) {
                    OnTouch((float)LOWORD(lParam), (float)HIWORD(lParam), 0);
                }
                return 0;

            case WM_KEYDOWN: {
                if (IsGameInitialized()) {
                    InputEvent evt{};
                    evt.device = InputDeviceType::Keyboard;
                    evt.action = InputAction::Down;
                    evt.keyCode = (int)wParam;
                    PushInputEvent(evt);
                }
                return 0;
            }
        }
    } catch (const std::exception& e) {
        MessageBoxA(hWnd, e.what(), "Error en WindowProc", MB_ICONERROR);
    } catch (...) {
        MessageBoxA(hWnd, "Excepción desconocida en WindowProc", "Error en WindowProc", MB_ICONERROR);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        FILE* initLog = fopen("Prueba3D_Trace.log", "w");
        if (initLog) {
            fprintf(initLog, "=== Prueba3D Windows Trace Started ===\n");
            fclose(initLog);
        }
        TraceLog("WinMain started");

        // Ajustar el directorio de trabajo a la ubicación del ejecutable para cargar los assets
        char exePath[MAX_PATH];
        if (GetModuleFileNameA(NULL, exePath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(exePath, '\\');
            if (lastSlash) {
                *lastSlash = '\0';
                SetCurrentDirectoryA(exePath);
                TraceLog("Working directory set to: %s", exePath);
            }
        }

        const char CLASS_NAME[] = "Prueba3DWindowClass";

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);

        HWND hWnd = CreateWindowEx(
            0, CLASS_NAME, "Prueba3D - Windows", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
            NULL, NULL, hInstance, NULL
        );

        if (hWnd == NULL) {
            TraceLog("ERROR: CreateWindowEx failed");
            MessageBoxA(NULL, "No se pudo crear la ventana de Windows.", "Error de Inicialización", MB_ICONERROR);
            return 0;
        }
        TraceLog("CreateWindowEx success: %p", hWnd);

        // 1. Mostrar la ventana inmediatamente con fondo sólido
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
        TraceLog("Window shown and updated");

        // 2. Intentar inicializar el motor gráfico
        TraceLog("Calling InitGame3D...");
        InitGame3D(hWnd, nullptr, 1280, 720);
        TraceLog("InitGame3D finished. IsGameInitialized(): %s", IsGameInitialized() ? "TRUE" : "FALSE");

        auto lastTime = std::chrono::high_resolution_clock::now();
        MSG msg = {};
        bool running = true;

        // 3. Bucle principal de Win32
        while (running && !ShouldQuit()) {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (!running) break;

            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            if (IsGameInitialized()) {
                UpdateGame3D(deltaTime);
            } else {
                // Si bgfx no se inicializó, pintar el texto de error nativo GDI en la ventana
                HDC hdc = GetDC(hWnd);
                RECT rect;
                GetClientRect(hWnd, &rect);
                HBRUSH brush = CreateSolidBrush(RGB(26, 26, 46));
                FillRect(hdc, &rect, brush);
                DeleteObject(brush);

                SetTextColor(hdc, RGB(255, 100, 100));
                SetBkMode(hdc, TRANSPARENT);
                const char* diagMsg = " ESTADO DE RENDERIZADO EN WINDOWS (main.cpp) \n\n"
                                      "[FALLO] bgfx::init() no se ha completado correctamente.\n\n"
                                      "Causas posibles:\n"
                                      "1. El contexto de ventana HWND no fue aceptado por el driver.\n"
                                      "2. El backend grafico (OpenGL) no esta disponible.\n"
                                      "3. Faltan librerias o soporte de GPU en el sistema.";
                DrawTextA(hdc, diagMsg, -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
                ReleaseDC(hWnd, hdc);
                Sleep(16);
            }
        }

        if (IsGameInitialized()) {
            ShutdownGame3D();
        }
    } catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Excepción en WinMain", MB_ICONERROR);
    } catch (...) {
        MessageBoxA(NULL, "Excepción fatal desconocida.", "Excepción en WinMain", MB_ICONERROR);
    }
    return 0;
}
