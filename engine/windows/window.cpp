#include "window.h"

namespace engine
{
Window::Window(HINSTANCE app_handle,
    uint16_t pos_x,
    uint16_t pos_y,
    uint16_t client_width,
    uint16_t client_height,
    uint8_t stretch_factor,
    float fps_limit) :
    m_input_listener(nullptr),
    m_position(pos_x, pos_y),
    m_client_size(client_width, client_height),
    m_stretch_factor(stretch_factor)
{
    Init(app_handle);
    InitPixelsBuffer();
    CreateSwapChain(fps_limit);
    CreateRenderTargetView();
}

void Window::AttachListener(InputListener *input_listener)
{
    m_input_listener = input_listener;
}

const glm::u16vec2 &Window::GetSize() const
{
    RECT size = { 0, 0, m_client_size.x, m_client_size.y };
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);

    return glm::u16vec2(size.right - size.left,
        size.bottom - size.top);
}

const glm::u16vec2 &Window::GetClientAreaSize() const
{
    return m_client_size;
}

const glm::u16vec2 &Window::GetPosition() const
{
    return m_position;
}

std::vector<uint32_t> &Window::GetPixelsBuffer()
{
    return m_pixels;
}

const glm::u16vec2 &Window::GetPixelsBufferSize() const
{
    return m_pixels_size;
}

float Window::GetStretchFactor() const
{
    return m_stretch_factor;
}

void Window::Move(uint16_t pos_x, uint16_t pos_y)
{
    m_position.x = pos_x;
    m_position.y = pos_y;
}

void Window::Resize(uint16_t width, uint16_t height)
{
    m_client_size.x = width;
    m_client_size.y = height;

    m_pixels_size.x = m_client_size.x / m_stretch_factor;
    m_pixels_size.y = m_client_size.y / m_stretch_factor;
    uint32_t size = m_pixels_size.x * m_pixels_size.y;

    m_bmp_info.bmiHeader.biWidth = m_pixels_size.x;
    m_bmp_info.bmiHeader.biHeight = -m_pixels_size.y; // inverted image
    m_bmp_info.bmiHeader.biSizeImage = 4 * size;

    m_pixels.resize(size);
}

void Window::Show(bool show)
{
    ShowWindow(m_handle, show ? SW_SHOW : SW_HIDE);
}

void Window::Clear()
{
    uint32_t size = m_pixels_size.x * m_pixels_size.y;

    for (uint32_t i = 0; i != size; ++i) m_pixels[i] = HEX_COLORS::BLACK;
}

void Window::Flush()
{
    StretchDIBits(m_hdc,
        0,
        0,
        m_client_size.x,
        m_client_size.y,
        0,
        0,
        m_pixels_size.x,
        m_pixels_size.y,
        m_pixels.data(),
        &m_bmp_info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

Window::~Window()
{
    ReleaseDC(m_handle, m_hdc);
}

void Window::InitWindowClass(HINSTANCE app_handle)
{
    ZeroMemory(&m_window_class, sizeof(WNDCLASSEX));
    m_window_class.cbSize = sizeof(WNDCLASSEX);
    m_window_class.style = CS_HREDRAW | CS_VREDRAW;
    m_window_class.lpfnWndProc = MessageRouter;
    m_window_class.hInstance = app_handle;
    m_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
    m_window_class.lpszClassName = L"window_class1";

    RegisterClassEx(&m_window_class);
}

void Window::Init(HINSTANCE app_handle)
{
    InitWindowClass(app_handle);

    glm::u16vec2 size = GetSize();

    m_handle = CreateWindowEx(NULL,
        L"window_class1",
        L"Engine",
        WS_OVERLAPPEDWINDOW,
        m_position.x,
        m_position.y,
        size.x,
        size.y,
        NULL,
        NULL,
        app_handle,
        this); // hack to make WindowProc class field

    m_hdc = GetDC(m_handle);
}

void Window::InitPixelsBuffer()
{
    m_pixels_size.x = m_client_size.x / m_stretch_factor;
    m_pixels_size.y = m_client_size.y / m_stretch_factor;

    uint32_t size = m_pixels_size.x * m_pixels_size.y;
    m_pixels.resize(size);

    ZeroMemory(&m_bmp_info, sizeof(BITMAPINFO));
    m_bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bmp_info.bmiHeader.biWidth = m_pixels_size.x;
    m_bmp_info.bmiHeader.biHeight = -m_pixels_size.y; // inverted image
    m_bmp_info.bmiHeader.biPlanes = 1;
    m_bmp_info.bmiHeader.biBitCount = 32;
    m_bmp_info.bmiHeader.biCompression = BI_RGB;
    m_bmp_info.bmiHeader.biSizeImage = 4 * size;

    // RGB
    for (uint32_t i = 0; i != size; ++i) m_pixels[i] = HEX_COLORS::BLACK;
}

void Window::CreateSwapChain(float fps_limit)
{
    const Direct3D *d3d = Direct3D::GetInstance();
    const Engine *engine = Engine::GetInstance();

    // Release the previous swapchain
    // (to be able to recreate swapchain on WM_SIZE for example):
    m_swapchain.Reset();

    DXGI_SWAP_CHAIN_DESC scd;
    scd.BufferDesc.Width = m_client_size.x;
    scd.BufferDesc.Height = m_client_size.y;
    scd.BufferDesc.RefreshRate.Numerator = fps_limit;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = engine->settings.backbuffer_format;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = engine->settings.swapchain_buffer_count;
    scd.OutputWindow = m_handle;
    scd.Windowed = true;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT result = d3d->m_factory->CreateSwapChain(d3d->m_command_queue.Get(),
                                                     &scd,
                                                     &m_swapchain);
    assert(result >= 0 && "Failed to CreateSwapChain()");
}

void Window::CreateRenderTargetView()
{
    const Direct3D *d3d = Direct3D::GetInstance();
    const Engine *engine = Engine::GetInstance();

    HRESULT result;

    // TODO: use CD3DX12_CPU_DESCRIPTOR_HANDLE wrapper which manage offsets in heap or write your own
    D3D12_CPU_DESCRIPTOR_HANDLE RTV_heap_handle = d3d->m_RTV_heap->GetCPUDescriptorHandleForHeapStart();

    // Create RTV for each buffer in swapchain
    for (uint32_t i = 0; i != engine->settings.swapchain_buffer_count; ++i)
    {
        wrl::ComPtr<ID3D12Resource> swapchain_buffer;
        
        result = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&swapchain_buffer));

        d3d->m_device->CreateRenderTargetView(swapchain_buffer.Get(), nullptr, RTV_heap_handle);

        // move to the next empty cell in the heap
        RTV_heap_handle.ptr += d3d->m_RTV_descriptor_size;
    }
}

void Window::CreateViewport()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_client_size.x);
    viewport.Height = static_cast<float>(m_client_size.y);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    d3d->m_command_list->RSSetViewports(1, &viewport);

    tagRECT mScissorRect = { 0, 0, m_client_size.x, m_client_size.y };
    d3d->m_command_list->RSSetScissorRects(1, &mScissorRect);
}

LRESULT CALLBACK Window::WindowProc(HWND win_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_MOVE:
    {
        uint16_t new_pos_x = LOWORD(l_param);
        uint16_t new_pos_y = HIWORD(l_param);

        Move(new_pos_x, new_pos_y);

        return 0;
    }
    case WM_SIZE:
    {
        uint16_t new_client_width = LOWORD(l_param);
        uint16_t new_client_height = HIWORD(l_param);

        Resize(new_client_width, new_client_height);

        return 0;
    }
    case WM_KEYDOWN:
    {
        VK_CODE key = static_cast<VK_CODE>(w_param);
        m_input_listener->onKeyDown(key);

        return 0;
    }
    case WM_KEYUP:
    {
        VK_CODE key = static_cast<VK_CODE>(w_param);
        m_input_listener->onKeyUp(key);

        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        m_input_listener->onKeyDown(VK_CODE::RMB);

        return 0;
    }
    case WM_RBUTTONUP:
    {
        m_input_listener->onKeyUp(VK_CODE::RMB);

        return 0;
    }
    } // switch

    return DefWindowProc(win_handle, message, w_param, l_param);
}

LRESULT CALLBACK Window::MessageRouter(HWND win_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    Window *win;

    if (message == WM_CREATE)
    {
        win = static_cast<Window *>(reinterpret_cast<LPCREATESTRUCT>(l_param)->lpCreateParams);
        SetWindowLongPtr(win_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(win));
    }
    else
    {
        win = reinterpret_cast<Window *>(GetWindowLongPtr(win_handle, GWLP_USERDATA));
    }

    if (win) return win->WindowProc(win_handle, message, w_param, l_param);
    else return DefWindowProc(win_handle, message, w_param, l_param); // skip all messages before WM_CREATE
}
} // engine