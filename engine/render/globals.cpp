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
    : m_dxgiFactory(nullptr)
    , m_device(nullptr)
    , m_deviceContext(nullptr)
#if defined(DEBUG) || defined(_DEBUG)
    , m_debug(nullptr)
#endif
    , m_linearSampler(nullptr)
{

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
        m_device.GetAddressOf(),
        nullptr,
        m_deviceContext.GetAddressOf());
    assert(hr >= 0 && "Failed to create device and device Context\n");

#if defined(DEBUG) || defined(_DEBUG)
    hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
    assert(hr >= 0 && "Unable to query ID3D11Debug\n");
#endif
}

void Globals::CreateSamplers()
{
    D3D11_SAMPLER_DESC linearSamplerDesc = {};
    linearSamplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    linearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    linearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    linearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

    HRESULT hr = m_device->CreateSamplerState(&linearSamplerDesc, m_linearSampler.GetAddressOf());
    assert(hr >= 0 && "Failed to create linear sampler state\n");
}

void Globals::BindSamplers()
{
    m_deviceContext->PSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->DSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
}

void Globals::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStateDesc = {};
    depthStateDesc.DepthEnable = TRUE;
    depthStateDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; // reversed depth
    depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    m_device->CreateDepthStencilState(&depthStateDesc, m_depthStencilState.GetAddressOf());
}

void Globals::BindDepthStencilState()
{
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
}

void Globals::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;

    m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf());
}

void Globals::BindRasterizerState()
{
    m_deviceContext->RSSetState(m_rasterizerState.Get());
}
} // namespace engine
