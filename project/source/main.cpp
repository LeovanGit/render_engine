#include <windows.h>
#include <iostream>
#include <memory>

#include "windows/window.h"
#include "application.h"
#include "utils/fps_timer.h"
#include "render/engine.h"

// after this std::cout will print to the created console
void initConsole()
{
    AllocConsole();
    FILE *dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout);
}

int WINAPI WinMain(HINSTANCE app_handle,
                   HINSTANCE prev_app_handle,
                   LPSTR cmd_line,
                   int window_show_params)
{
    initConsole();

    engine::Engine::Init();

    FPSTimer fps_timer(60.0f);

    Application app(app_handle);
    app.m_window.Show();

    MSG msg;
    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) goto exit;
        }

        if (fps_timer.IsFrameTimeElapsed())
        {
            float delta_time = fps_timer.GetDeltaTime();

            // float fps = 1.0f / delta_time;
            // std::cout << "FPS: " << fps << "\n";

            app.ProcessInput(delta_time);
            app.m_scene.Render(app.m_window);
            app.m_window.Flush();
        }
    }

exit:
    engine::Engine::Deinit();

    return static_cast<int>(msg.wParam);
}
