#pragma once

#include <iostream>

#include "utils/utils.h"
#include "utils/d3dcommon.h"

namespace engine
{
namespace
{
// SETTINGS
const uint32_t s_swapchainBuffersCount = 2;
const DXGI_FORMAT s_backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
const DXGI_FORMAT s_depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
}

class Globals : public NonCopyable
{
public:
    static void Create();
    static Globals *GetInstance();
    static void Destroy();

    void InitD3D12();

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptor(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptor(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCBV_SRV_UAVDescriptor(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetSamplerDescriptor(uint32_t index) const;

    uint32_t GetNextFreeCBV_SRV_UAVDescriptorIndex()
    {
        return m_CBV_SRV_UAVDescriptorIndex++;
    }

    void BindDescriptorHeaps() const;

    void BindConstantBuffers() const;
    void BindSRVDescriptor(uint32_t slotInHeap) const;
    void BindSamplers() const;

    void BeginCommandsRecording();
    void EndCommandsRecording();
    void Submit();
    void FlushCommandQueue();

    void CreateGlobalRootSignature();
    void BindGlobalRootSignature();

    void CreateSamplers();

    ComPtr<IDXGIFactory6> m_dxgiFactory;
    ComPtr<ID3D12Device> m_device;

#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12Debug1> m_debug;
#endif

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12Fence> m_fence;
    uint64_t m_fenceValue;

    uint32_t m_RTVDescriptorSize;
    uint32_t m_DSVDescriptorSize;
    uint32_t m_CBV_SRV_UAVDescriptorSize;
    uint32_t m_samplerDescriptorSize;

    ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
    ComPtr<ID3D12DescriptorHeap> m_CBV_SRV_UAVHeap;
    ComPtr<ID3D12DescriptorHeap> m_samplersHeap;

    uint32_t m_CBV_SRV_UAVDescriptorIndex;

    ComPtr<ID3D12RootSignature> m_globalRootSignature;

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
