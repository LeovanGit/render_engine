#include "globals.h"

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
#if defined(DEBUG) || defined(_DEBUG)
    , m_debug(nullptr)
#endif
    , m_commandQueue(nullptr)
    , m_allocator(nullptr)
    , m_commandList(nullptr)
    , m_fence(nullptr)
    , m_fenceValue(0)
    , m_RTVHeap(nullptr)
    , m_DSVHeap(nullptr)
    //, m_linearSampler(nullptr)
{
    InitD3D12();
    CreateSamplers();
    CreateDepthStencilState();
    CreateRasterizerState();
}

Globals::~Globals()
{

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
        //OutputDebugStringW(text.c_str());
        std::wcout << text;

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

            //OutputDebugStringW(text.c_str());
            std::wcout << text;
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
        //OutputDebugStringW(text.c_str());
        std::wcout << text;

        LogAdapterOutputs(adapter.Get());

        ++i;
    }

    OutputDebugStringW(L"\n");
}

void Globals::InitD3D12()
{
    HRESULT hr;

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
    assert(hr >= 0 && "Unable to create DXGIFactory\n");

    LogAdapters();

#if defined(DEBUG) || defined(_DEBUG)
    hr = D3D12GetDebugInterface(IID_PPV_ARGS(m_debug.GetAddressOf()));
    assert(hr >= 0 && "Unable to create ID3D12Debug\n");

    m_debug->EnableDebugLayer();
#endif

    hr = D3D12CreateDevice(
        nullptr, // use default display adapter
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(m_device.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12Device\n");

    CreateCommandObjects();

    hr = m_device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(m_fence.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12Fence\n");

    m_RTVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DSVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CBV_SRV_UAVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    CreateDescriptorHeaps();
}

void Globals::CreateCommandObjects()
{
    HRESULT hr;

    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cmdQueueDesc.NodeMask = 0;

    hr = m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12CommandQueue\n");

    hr = m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(m_allocator.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12CommandAllocator\n");

    hr = m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_allocator.Get(),
        nullptr, // PSO
        IID_PPV_ARGS(m_commandList.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12GraphicsCommandList\n");

    m_commandList->Close();
}

void Globals::CreateDescriptorHeaps()
{
    HRESULT hr;

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = 2; // count of swapchain buffers
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RTVHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DSVHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetRTVDescriptor(uint32_t index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_RTVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_RTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetDSVDescriptor(uint32_t index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_DSVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_DSVDescriptorSize);
}

void Globals::BeginCommandsRecording()
{
    HRESULT hr;

    // before reset make sure that GPU has finished execution of previous commands:
    hr = m_allocator->Reset();
    assert(hr >= 0 && "Failed to reset ID3D12CommandAllocator\n");

    hr = m_commandList->Reset(m_allocator.Get(), nullptr);
    assert(hr >= 0 && "Failed to reset ID3D12GraphicsCommandList\n");
}

void Globals::EndCommandsRecording()
{
    HRESULT hr = m_commandList->Close();
    assert(hr >= 0 && "Failed to close ID3D12GraphicsCommandList\n");
}

void Globals::Submit()
{
    ID3D12CommandList *cmdsLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

// CPU wait until GPU complete frame commands
void Globals::FlushCommandQueue()
{
    HRESULT hr;

    m_fenceValue++;
    hr = m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
    assert(hr >= 0 && "Failed to add fence point\n");

    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        hr = m_fence->SetEventOnCompletion(m_fenceValue, hEvent);
        assert(hr >= 0 && "Failed to set event\n");

        WaitForSingleObject(hEvent, INFINITE);

        CloseHandle(hEvent);
    }
}

void Globals::CreateSamplers()
{
    /*D3D11_SAMPLER_DESC linearSamplerDesc = {};
    linearSamplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    linearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    linearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    linearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
    linearSamplerDesc.MipLODBias = 0.0f;
    linearSamplerDesc.MaxAnisotropy = 1;
    linearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    HRESULT hr = m_device->CreateSamplerState(&linearSamplerDesc, m_linearSampler.GetAddressOf());
    assert(hr >= 0 && "Failed to create sampler state\n");*/
}

void Globals::BindSamplers()
{
    /*m_deviceContext->VSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->HSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->DSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->GSSetSamplers(0, 1, m_linearSampler.GetAddressOf());
    m_deviceContext->PSSetSamplers(0, 1, m_linearSampler.GetAddressOf());

    m_deviceContext->CSSetSamplers(0, 1, m_linearSampler.GetAddressOf());*/
}

void Globals::CreateDepthStencilState()
{
    /*D3D11_DEPTH_STENCIL_DESC depthStateDesc = {};
    depthStateDesc.DepthEnable = TRUE;
    depthStateDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; // reversed depth
    depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    m_device->CreateDepthStencilState(&depthStateDesc, m_depthStencilState.GetAddressOf());*/
}

void Globals::BindDepthStencilState()
{
    //m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
}

void Globals::CreateRasterizerState()
{
    /*D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;

    m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf());*/
}

void Globals::BindRasterizerState()
{
    //m_deviceContext->RSSetState(m_rasterizerState.Get());
}
} // namespace engine
