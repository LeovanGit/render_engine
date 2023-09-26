#pragma once

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>

#include <dxgi.h>
#include <wrl.h> // Windows Runtime Library -> ComPtr

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

    void InitDirect3D();

    void LogAvailableVideoAdapters();

    // docs recommend to use DXGI things with "1" suffix for D3D11+
    wrl::ComPtr<IDXGIFactory1> m_factory;
};
} // namespace engine
