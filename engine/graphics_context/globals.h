#pragma once

#include "utils/utils.h"
#include "utils/d3d11common.h"

namespace engine
{
class Globals : public NonCopyable
{
public:
    static void Create();
    static Globals *GetInstance();
    static void Destroy();

    void InitD3D11();

    void CreateSamplers();
    void BindSamplers();

    void CreateDepthStencilState();
    void BindDepthStencilState();

    void CreateRasterizerState();
    void BindRasterizerState();

    ComPtr<IDXGIFactory2> m_dxgiFactory;
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;

#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D11Debug> m_debug;
#endif

    ComPtr<ID3D11SamplerState> m_linearSampler;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;

private:
    Globals();
    ~Globals() = default;

    static Globals *s_instance;
};
} // namespace engine
