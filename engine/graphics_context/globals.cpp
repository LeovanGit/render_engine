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
#if defined(DEBUG) || defined(_DEBUG)
    , m_debug(nullptr)
#endif
    , m_commandQueue(nullptr)
    , m_allocator(nullptr)
    , m_commandList(nullptr)
    , m_fence(nullptr)
    , m_fenceValue(0)
    , m_RTVDescriptorSize(0)
    , m_DSVDescriptorSize(0)
    , m_CBV_SRV_UAVDescriptorSize(0)
    , m_samplerDescriptorSize(0)
    , m_RTVHeap(nullptr)
    , m_DSVHeap(nullptr)
    , m_CBVHeap(nullptr)
    , m_SRVHeap(nullptr)
    , m_samplersHeap(nullptr)
    , m_globalRootSignature(nullptr)
{
    InitD3D12();

    CreateGlobalRootSignature();

    CreateSamplers();
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

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.NumDescriptors = 1; // 1 texture
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SRVHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");

    D3D12_DESCRIPTOR_HEAP_DESC samplersHeapDesc = {};
    samplersHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplersHeapDesc.NumDescriptors = 1; // 1 sampler
    samplersHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    samplersHeapDesc.NodeMask = 0;

    hr = m_device->CreateDescriptorHeap(&samplersHeapDesc, IID_PPV_ARGS(m_samplersHeap.GetAddressOf()));
    assert(hr >= 0 && "Failed to create ID3D12DescriptorHeap\n");

    m_RTVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DSVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CBV_SRV_UAVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_samplerDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetRTVDescriptor(uint32_t index) const
{
    assert(index < m_RTVHeap->GetDesc().NumDescriptors && "index >= NumDescriptors");

    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_RTVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_RTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetDSVDescriptor(uint32_t index) const
{
    assert(index < m_DSVHeap->GetDesc().NumDescriptors && "index >= NumDescriptors");

    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_DSVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_DSVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetCBVDescriptor(uint32_t index) const
{
    assert(index < m_CBVHeap->GetDesc().NumDescriptors && "index >= NumDescriptors");

    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_CBVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_CBV_SRV_UAVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetSRVDescriptor(uint32_t index) const
{
    assert(index < m_SRVHeap->GetDesc().NumDescriptors && "index >= NumDescriptors");

    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_SRVHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_CBV_SRV_UAVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Globals::GetSamplerDescriptor(uint32_t index) const
{
    assert(index < m_samplersHeap->GetDesc().NumDescriptors && "index >= NumDescriptors");

    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_samplersHeap->GetCPUDescriptorHandleForHeapStart(),
        index,
        m_samplerDescriptorSize);
}

void Globals::BindCBVDescriptorsHeap() const
{
    m_commandList->SetDescriptorHeaps(1, m_CBVHeap.GetAddressOf());
}

void Globals::BindSRVDescriptorsHeap() const
{
    m_commandList->SetDescriptorHeaps(1, m_SRVHeap.GetAddressOf());
}

void Globals::BindSamplerDescriptorsHeap() const
{
    m_commandList->SetDescriptorHeaps(1, m_samplersHeap.GetAddressOf());
}

void Globals::BindConstantBuffers() const
{
    BindCBVDescriptorsHeap();

    m_commandList->SetGraphicsRootDescriptorTable(
        RootSignatureSlot_CBV,
        m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
}

void Globals::BindShaderResources() const
{
    BindSRVDescriptorsHeap();

    m_commandList->SetGraphicsRootDescriptorTable(
        RootSignatureSlot_SRV,
        m_SRVHeap->GetGPUDescriptorHandleForHeapStart());
}

void Globals::BindSamplers() const
{
    BindSamplerDescriptorsHeap();

    m_commandList->SetGraphicsRootDescriptorTable(
        RootSignatureSlot_Sampler,
        m_samplersHeap->GetGPUDescriptorHandleForHeapStart());
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

void Globals::CreateGlobalRootSignature()
{
    HRESULT hr;

    // DESCRIPTOR RANGES
    D3D12_DESCRIPTOR_RANGE CBVDescriptorRange = {};
    CBVDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    CBVDescriptorRange.NumDescriptors = 1;
    CBVDescriptorRange.BaseShaderRegister = 0; // b0
    CBVDescriptorRange.RegisterSpace = 0;
    CBVDescriptorRange.OffsetInDescriptorsFromTableStart = 0;

    D3D12_DESCRIPTOR_RANGE SRVDescriptorRange = {};
    SRVDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    SRVDescriptorRange.NumDescriptors = 1;
    SRVDescriptorRange.BaseShaderRegister = 0; // t0
    SRVDescriptorRange.RegisterSpace = 0;
    SRVDescriptorRange.OffsetInDescriptorsFromTableStart = 0;

    D3D12_DESCRIPTOR_RANGE samplerDescriptorRange = {};
    samplerDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    samplerDescriptorRange.NumDescriptors = 1;
    samplerDescriptorRange.BaseShaderRegister = 0; // s0
    samplerDescriptorRange.RegisterSpace = 0;
    samplerDescriptorRange.OffsetInDescriptorsFromTableStart = 0;

    // DESCRIPTOR TABLES
    D3D12_ROOT_DESCRIPTOR_TABLE descriptorTables[3];
    descriptorTables[0].NumDescriptorRanges = 1;
    descriptorTables[0].pDescriptorRanges = &CBVDescriptorRange;

    descriptorTables[1].NumDescriptorRanges = 1;
    descriptorTables[1].pDescriptorRanges = &SRVDescriptorRange;

    descriptorTables[2].NumDescriptorRanges = 1;
    descriptorTables[2].pDescriptorRanges = &samplerDescriptorRange;

    // ROOT SIGNATURE
    D3D12_ROOT_PARAMETER rootParameters[RootSignatureSlot_Count];
    rootParameters[RootSignatureSlot_CBV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[RootSignatureSlot_CBV].DescriptorTable = descriptorTables[0];
    rootParameters[RootSignatureSlot_CBV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[RootSignatureSlot_SRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[RootSignatureSlot_SRV].DescriptorTable = descriptorTables[1];
    rootParameters[RootSignatureSlot_SRV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[RootSignatureSlot_Sampler].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[RootSignatureSlot_Sampler].DescriptorTable = descriptorTables[2];
    rootParameters[RootSignatureSlot_Sampler].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
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

void Globals::BindGlobalRootSignature()
{
    m_commandList->SetGraphicsRootSignature(m_globalRootSignature.Get());
}

void Globals::CreateSamplers()
{
    D3D12_SAMPLER_DESC linearSamplerDesc = {};
    linearSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    linearSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    linearSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    linearSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    linearSamplerDesc.MipLODBias = 0.0f;
    linearSamplerDesc.MaxAnisotropy = 1;
    linearSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

    m_device->CreateSampler(&linearSamplerDesc, GetSamplerDescriptor(0));
}
} // namespace engine
