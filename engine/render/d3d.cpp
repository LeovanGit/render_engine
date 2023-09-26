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
}

void Direct3D::CreateFactory()
{
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
	assert(result >= 0 && "Failed to CreateDXGIFactory()");
}

void Direct3D::EnableDebugLayer()
{
    wrl::ComPtr<ID3D12Debug> debugController;

	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	assert(result >= 0 && "Failed to D3D12GetDebugInterface()");

	debugController->EnableDebugLayer();
}

void Direct3D::CreateDevice()
{
	HRESULT result;

	// Try to create hardware device (nullptr means default adapter)
	result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

	// If failed then try to create WARP (software) device
	if (FAILED(result))
	{
		wrl::ComPtr<IDXGIAdapter> adapterWARP;

		result = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&adapterWARP));
		assert(result >= 0 && "Failed to EnumWarpAdapter()");

		result = D3D12CreateDevice(adapterWARP.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		assert(result >= 0 && "Failed to D3D12CreateDevice()");
	}
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
