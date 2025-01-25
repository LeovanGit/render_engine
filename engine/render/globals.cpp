#include "globals.h"

#include <cassert>

namespace engine
{
Globals *Globals::s_instance = nullptr;

void Globals::Create()
{
    assert(s_instance == nullptr && "Globals instance already created\n");
    s_instance = new Globals;
}

Globals *Globals::GetInstance()
{
    assert(s_instance && "Globals instance is not created\n");
    return s_instance;
}

void Globals::Destroy()
{
    assert(s_instance && "Globals instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

Globals::Globals()
    : m_debug(nullptr)
    , m_dxgiFactory(nullptr)
    , m_device(nullptr)
    , m_deviceContext(nullptr)
{

}

Globals::~Globals()
{
    m_deviceContext.Reset();
#if defined(DEBUG) || defined(_DEBUG)
    // check that all D3D11 objects was released (warning in output):
    m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    m_debug.Reset();
#endif
    m_device.Reset();
}

void Globals::InitD3D11()
{
    HRESULT hr;

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
    assert(hr >= 0 && "Unable to create DXGIFactory\n");

#if defined(DEBUG) || defined(_DEBUG)
    UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT flags = 0;
#endif

    constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &m_device,
        nullptr,
        &m_deviceContext);
    assert(hr >= 0 && "Failed to create device and device Context\n");

#if defined(DEBUG) || defined(_DEBUG)
    hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
    assert(hr >= 0 && "Unable to query ID3D11Debug\n");
#endif
}
} // namespace engine
