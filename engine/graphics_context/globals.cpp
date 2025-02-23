#include "globals.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <string>

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
    InitD3D11();
    CreateSamplers();
    CreateDepthStencilState();
    CreateRasterizerState();
}

void Globals::LogAdapterOutputs(IDXGIAdapter *adapter)
{
    uint32_t i = 0;
    ComPtr<IDXGIOutput> output = nullptr;

    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"Adapter Output: ";
        text += desc.DeviceName;
        text += L"\n";
        OutputDebugStringW(text.c_str());

        // Call with nullptr to get count:
        uint32_t count = 0;
        output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &count, nullptr);

        std::vector<DXGI_MODE_DESC> displayModes(count);
        output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &count, &displayModes[0]);

        for (auto &displayMode : displayModes)
        {
            float refreshRate = float(displayMode.RefreshRate.Numerator) / displayMode.RefreshRate.Denominator;

            std::wstring text = 
                std::to_wstring(displayMode.Width) + L"x" +
                std::to_wstring(displayMode.Height) + L" " +
                std::to_wstring(refreshRate) + L" Hz\n";

            OutputDebugStringW(text.c_str());
        }

        ++i;
    }
}

void Globals::LogAdapters()
{
    uint32_t i = 0;
    ComPtr<IDXGIAdapter> adapter = nullptr;

    while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"\nAdapter: ";
        text += desc.Description;
        text += L"\n";
        OutputDebugStringW(text.c_str());

        LogAdapterOutputs(adapter.Get());

        ++i;
    }

    OutputDebugStringW(L"\n");
}

void Globals::InitD3D11()
{
    HRESULT hr;

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
    assert(hr >= 0 && "Unable to create DXGIFactory\n");

    LogAdapters();

#if defined(DEBUG) || defined(_DEBUG)
    UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    UINT flags = 0;
#endif

    constexpr D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL selectedFeatureLevel;

    hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        &featureLevels[0],
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_device.GetAddressOf(),
        &selectedFeatureLevel,
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
    linearSamplerDesc.MipLODBias = 0.0f;
    linearSamplerDesc.MaxAnisotropy = 1;
    linearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    HRESULT hr = m_device->CreateSamplerState(&linearSamplerDesc, m_linearSampler.GetAddressOf());
    assert(hr >= 0 && "Failed to create sampler state\n");
}

void Globals::BindSamplers()
{
    m_deviceContext->VSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->HSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->DSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->GSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->PSSetSamplers(0, 1, m_linearSampler.GetAddressOf());

    m_deviceContext->CSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
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
