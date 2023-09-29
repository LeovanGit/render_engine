#include "d3d.h"

namespace engine
{
Direct3D *Direct3D::m_instance = nullptr;

void Direct3D::Init()
{
    assert(!m_instance && "Direct3D::Init() was called twice!");
    
    m_instance = new Direct3D();

	m_instance->InitDirect3D12();
	m_instance->LogAvailableVideoAdapters();
}

const Direct3D *Direct3D::GetInstance()
{
    assert(m_instance && "Direct3D singleton is not initialized!");

    return m_instance;
}

void Direct3D::Deinit()
{
    assert(m_instance && "Direct3D singleton is not initalized or Direct3D::Deinit() was called twice!!");

    delete m_instance;
    m_instance = nullptr;
}

void Direct3D::InitDirect3D12()
{
	CreateFactory();

#if defined(DEBUG) || defined(_DEBUG)
	EnableDebugLayer();
#endif

	CreateDevice();
	CreateFence();
	CreateCommandObjects();
	GetDescriptorSizes();
	CreateDescriptorHeaps();
}

void Direct3D::EnableDebugLayer()
{
	wrl::ComPtr<ID3D12Debug> debug_controller;

	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
	assert(result >= 0 && "Failed to D3D12GetDebugInterface()");

	debug_controller->EnableDebugLayer();
}

void Direct3D::CreateFactory()
{
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
	assert(result >= 0 && "Failed to CreateDXGIFactory()");
}

void Direct3D::CreateDevice()
{
	HRESULT result;

	// Try to create hardware device (nullptr means default adapter)
	result = D3D12CreateDevice(nullptr,
	                           D3D_FEATURE_LEVEL_11_0,
		                       IID_PPV_ARGS(&m_device));

	// If failed then try to create WARP (software) device
	if (FAILED(result))
	{
		wrl::ComPtr<IDXGIAdapter> adapterWARP;

		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapterWARP));
		assert(result >= 0 && "Failed to EnumWarpAdapter()");

		result = D3D12CreateDevice(adapterWARP.Get(),
			                       D3D_FEATURE_LEVEL_11_0,
			                       IID_PPV_ARGS(&m_device));
		assert(result >= 0 && "Failed to D3D12CreateDevice()");
	}
}

void Direct3D::CreateFence()
{
	HRESULT result = m_device->CreateFence(0,
		                                   D3D12_FENCE_FLAG_NONE,
		                                   IID_PPV_ARGS(&m_fence));
	assert(result >= 0 && "Failed to CreateFence()");
}

void Direct3D::CreateCommandObjects()
{
	HRESULT result;

	D3D12_COMMAND_QUEUE_DESC cqd = {};
	cqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result = m_device->CreateCommandQueue(&cqd,
		                                  IID_PPV_ARGS(&m_command_queue));
	assert(result >= 0 && "Failed to CreateCommandQueue()");

	result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		                                      IID_PPV_ARGS(&m_command_list_allocator));
	assert(result >= 0 && "Failed to CreateCommandAllocator()");

	result = m_device->CreateCommandList(0,
		                                 D3D12_COMMAND_LIST_TYPE_DIRECT,
		                                 m_command_list_allocator.Get(),
		                                 nullptr, // dummy PipelineState
		                                 IID_PPV_ARGS(&m_command_list));
    assert(result >= 0 && "Failed to CreateCommandList()");

	// close command list because we will first call Reset() in other methods
    m_command_list->Close();
}

void Direct3D::GetDescriptorSizes()
{
	m_RTV_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_DSV_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	m_CBV_SRV_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_sampler_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

void Direct3D::CreateDescriptorHeaps()
{
	const Engine *engine = Engine::GetInstance();

	HRESULT result;

	// D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
	rtv_heap_desc.NumDescriptors = engine->settings.swapchain_buffer_count;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0;

	result = m_device->CreateDescriptorHeap(&rtv_heap_desc,
		                                    IID_PPV_ARGS(&m_RTV_heap));
	assert(result >= 0 && "Failed to CreateDescriptorHeap()");

	// D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;

	result = m_device->CreateDescriptorHeap(&dsv_heap_desc,
		                                    IID_PPV_ARGS(&m_DSV_heap));
	assert(result >= 0 && "Failed to CreateDescriptorHeap()");

	// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	D3D12_DESCRIPTOR_HEAP_DESC cbv_srv_uav_heap_desc;
	cbv_srv_uav_heap_desc.NumDescriptors = 1;
	cbv_srv_uav_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbv_srv_uav_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbv_srv_uav_heap_desc.NodeMask = 0;

	result = m_device->CreateDescriptorHeap(&cbv_srv_uav_heap_desc,
		                                    IID_PPV_ARGS(&m_CBV_SRV_UAV_heap));
	assert(result >= 0 && "Failed to CreateDescriptorHeap()");

	// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
	D3D12_DESCRIPTOR_HEAP_DESC sampler_heap_desc;
	sampler_heap_desc.NumDescriptors = 1;
	sampler_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	sampler_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	sampler_heap_desc.NodeMask = 0;

	result = m_device->CreateDescriptorHeap(&sampler_heap_desc,
	                                     	IID_PPV_ARGS(&m_sampler_heap));
	assert(result >= 0 && "Failed to CreateDescriptorHeap()");
}

// TODO: rename to GetCPUHandle?
D3D12_CPU_DESCRIPTOR_HANDLE Direct3D::GetCurrentBackBufferView() const
{
	// TODO: move this maybe to window and change value 0-1 on swapchain swaps:
	uint8_t current_backbuffer = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTV_heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_RTV_descriptor_size * current_backbuffer;

	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Direct3D::GetDepthStencilView() const
{
	return m_DSV_heap->GetCPUDescriptorHandleForHeapStart();
}

void Direct3D::LogAvailableVideoAdapters()
{
	std::cout << "==========[AVAILABLE VIDEO ADAPTERS]==========\n";

	uint32_t i = 0;
	IDXGIAdapter1 *adapter = nullptr;
	
	while (m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wcout << desc.Description << "\n";
		//LogVideoAdapterOutputs(adapter);

		++i;
	}

	std::cout << "==============================================\n";
}

void Direct3D::LogVideoAdapterOutputs(IDXGIAdapter1 *adapter)
{
	uint32_t i = 0;
	IDXGIOutput *output = nullptr;

	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wcout << desc.DeviceName << "\n";

		LogOutputDisplayModes(output, DXGI_FORMAT_B8G8R8A8_UNORM);

		++i;
	}
}

void Direct3D::LogOutputDisplayModes(IDXGIOutput *output, DXGI_FORMAT format)
{
	// call with nullptr to get count
	uint32_t count = 0;
	output->GetDisplayModeList(format, 0, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, 0, &count, &modeList[0]);

	for (auto &mode : modeList)
	{
		float refreshRate = float(mode.RefreshRate.Numerator) / mode.RefreshRate.Denominator;

		std::wcout << mode.Width << "x" << mode.Height << " " << refreshRate << "\n";
	}
}
} // namespace engine
