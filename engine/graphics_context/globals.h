#pragma once

#include <iostream>

#include "utils/utils.h"
#include "utils/d3dcommon.h"

namespace engine
{
class Globals : public NonCopyable
{
public:
    static void Create();
    static Globals *GetInstance();
    static void Destroy();

    void InitD3D12();

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptor(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptor(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCBVDescriptor(uint32_t index) const;

    void BeginCommandsRecording();
    void EndCommandsRecording();
    void Submit();
    void FlushCommandQueue();

    void CreateRootSignature();
    void BindRootSignature();

    void CreateSamplers();
    void BindSamplers();

    void CreateBlendState();
    void CreateDepthStencilState();
    void CreateRasterizerState();

    void BindCBVDescriptorsHeap();

    void BindCBV();

    ComPtr<IDXGIFactory6> m_dxgiFactory;
    ComPtr<ID3D12Device> m_device;

#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12Debug> m_debug;
#endif

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12Fence> m_fence;
    uint64_t m_fenceValue;

    uint32_t m_RTVDescriptorSize;
    uint32_t m_DSVDescriptorSize;
    uint32_t m_CBV_SRV_UAVDescriptorSize;

    ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
    ComPtr<ID3D12DescriptorHeap> m_CBVHeap;

    D3D12_BLEND_DESC m_blendState;
    D3D12_DEPTH_STENCIL_DESC m_depthStencilState;
    D3D12_RASTERIZER_DESC m_rasterizerState;

    ComPtr<ID3D12RootSignature> m_globalRootSignature;
    
    //ComPtr<ID3D11SamplerState> m_linearSampler;

private:
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter *adapter);

    void CreateCommandObjects();
    void CreateDescriptorHeaps();

    Globals();
    ~Globals() = default;

    static Globals *s_instance;
};
} // namespace engine
