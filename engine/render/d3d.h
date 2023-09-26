#pragma once

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>

#include <dxgi1_4.h>
#include <wrl.h> // Windows Runtime Library -> ComPtr
#include <d3d12.h>

#include "../source/non_copyable.h"

namespace wrl = Microsoft::WRL;

namespace engine
{
class Direct3D final : public NonCopyable
{
public:
    static void Init();

    static const Direct3D *GetInstance();

    static void Deinit();

private:
    Direct3D() = default;
    ~Direct3D() = default;

    static Direct3D *m_instance;

    void InitDirect3D12();
    void CreateFactory();
    void EnableDebugLayer();
    void CreateDevice();

    void LogAvailableVideoAdapters();

    void LogVideoAdapterOutputs(IDXGIAdapter1 *adapter);

    void LogOutputDisplayModes(IDXGIOutput *output, DXGI_FORMAT format);

    // our engine requires DirectX12 support, so we should use IDXGIFactory4
    wrl::ComPtr<IDXGIFactory4> m_factory;
    wrl::ComPtr<ID3D12Device> m_device;
};
} // namespace engine
