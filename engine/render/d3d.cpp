#include "d3d.h"

namespace engine
{
Direct3D *Direct3D::m_instance = nullptr;

void Direct3D::Init()
{
    assert(!m_instance && "Direct3D::Init() was called twice!");
    
    m_instance = new Direct3D();

	m_instance->InitDirect3D();
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

void Direct3D::InitDirect3D()
{
	HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), &m_factory);
	assert(result >= 0 && "Failed to CreateDXGIFactory()");
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

		++i;
	}

	std::cout << "==============================================\n";
}
} // namespace engine
