#include <winrt/Windows.UI.Xaml.Controls.h>
#include "game.h"      // Tus funciones InitGame3D, etc.

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    init_apartment();

    InitGame3D();  // Inicializa tu motor core

    Application::Start([](auto &&) {
        // Aquí se inicia tu MainPage XAML automáticamente
    });

    ShutdownGame3D(); // Limpieza al cerrar
}