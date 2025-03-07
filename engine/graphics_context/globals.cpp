#include "globals.h"

#include <vector>
#include <cassert>
#include <string>
#include <unordered_map>

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
    , m_CBVHeap(nullptr)
    //, m_linearSampler(nullptr)
{
    InitD3D12();
    CreateSamplers();
    CreateBlendState();
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
    rtvHeapDesc.NumDescriptors = 2; // 2 swapchain buffers
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RTVHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.NumDescriptors = 1; // 1 depth buffer
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DSVHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.NumDescriptors = 1; // 1 constant buffer
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(m_CBVHeap.GetAddressOf()));
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

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetCBVDescriptor(uint32_t index) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_CBVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_CBV_SRV_UAVDescriptorSize);
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

void Globals::CreateRootSignature()
{
    HRESULT hr;

    D3D12_DESCRIPTOR_RANGE descriptorRange;
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    descriptorRange.NumDescriptors = 1;
    descriptorRange.BaseShaderRegister = 0;
    descriptorRange.RegisterSpace = 0;
    descriptorRange.OffsetInDescriptorsFromTableStart = 0;

    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
    descriptorTable.NumDescriptorRanges = 1;
    descriptorTable.pDescriptorRanges = &descriptorRange;

    D3D12_ROOT_PARAMETER rootParameters;
    rootParameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters.DescriptorTable = descriptorTable;
    rootParameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 1;
    rootSignatureDesc.pParameters = &rootParameters;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
    ComPtr<ID3DBlob> error = nullptr;

    hr = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        rootSignatureBlob.GetAddressOf(),
        error.GetAddressOf());

    if (hr < 0)
    {
        if (error)
        {
            std::wcout << static_cast<char *>(error->GetBufferPointer()) << "\n";
        }

        assert(false && "Failed to serialize root signature\n");
    }

    hr = m_device->CreateRootSignature(
        0,
        rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(),
        IID_PPV_ARGS(m_globalRootSignature.GetAddressOf()));
    assert(hr >= 0 && "Failed to create root signature\n");
}

void Globals::BindRootSignature()
{
    m_commandList->SetGraphicsRootSignature(m_globalRootSignature.Get());
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

void Globals::CreateBlendState()
{
    m_blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
}

void Globals::CreateDepthStencilState()
{
    m_depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    m_depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; // reversed depth
}

void Globals::CreateRasterizerState()
{
    m_rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
}

void Globals::BindCBVDescriptorsHeap()
{
    ID3D12DescriptorHeap *descriptorHeaps[] = { m_CBVHeap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void Globals::BindCBV()
{
    m_commandList->SetGraphicsRootDescriptorTable(
        0,
        m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
}
} // namespace engine
