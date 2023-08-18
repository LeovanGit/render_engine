#include "window.h"

Window::Window(HINSTANCE hInstance,
               int posX,
               int posY,
               int width,
               int height) :
               posX(posX),
               posY(posY),
               width(width),
               height(height)
{
    Init(hInstance);
}

RECT Window::GetSize()
{
    RECT size = { 0, 0, width, height };

    return size;
}

RECT Window::GetClientAreaSize()
{
    RECT size = { 0, 0, width, height };
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);

    return size;
}

void Window::Show(bool show)
{
    ShowWindow(handle, show ? SW_SHOW : SW_HIDE);
}

void Window::InitWindowClass(HINSTANCE hInstance)
{
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    windowClass.lpszClassName = L"WindowClass1";

    RegisterClassEx(&windowClass);
}

void Window::Init(HINSTANCE hInstance)
{
    InitWindowClass(hInstance);

    RECT size = GetClientAreaSize();

    handle = CreateWindowEx(NULL,
                            L"WindowClass1",
                            L"Engine",
                            WS_OVERLAPPEDWINDOW,
                            posX,
                            posY,
                            size.right - size.left,
                            size.bottom - size.top,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);
}

LRESULT CALLBACK Window::WindowProc(HWND hWin, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hWin, message, wParam, lParam);
}