#include <windows.h>

#include "source/windows/window.h"

//extern Window window;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    Window window(hInstance, 400, 400, 400, 300);
    window.Show();

    MSG msg;

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }
        else
        {
            // main loop

        }
    }

    return static_cast<int>(msg.wParam);
}