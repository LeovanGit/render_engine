#pragma once

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>

#include "../source/stdafx.h"
#include "engine.h"

#include "../source/non_copyable.h"

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
    void EnableDebugLayer();
    void CreateFactory();
    void CreateDevice();
    void CreateFence();
    void CreateCommandObjects();
    void GetDescriptorSizes();
    void CreateDescriptorHeaps();

    void LogAvailableVideoAdapters();
    void LogVideoAdapterOutputs(IDXGIAdapter1 *adapter);
    void LogOutputDisplayModes(IDXGIOutput *output, DXGI_FORMAT format);

public:
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    // our engine requires DirectX12 support, so we should use IDXGIFactory4
    wrl::ComPtr<IDXGIFactory4> m_factory;
    wrl::ComPtr<ID3D12Device> m_device;
    wrl::ComPtr<ID3D12Fence> m_fence;

    wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
    wrl::ComPtr<ID3D12CommandAllocator> m_command_list_allocator;
    wrl::ComPtr<ID3D12GraphicsCommandList> m_command_list;

    uint32_t m_RTV_descriptor_size;
    uint32_t m_DSV_descriptor_size;
    uint32_t m_CBV_SRV_descriptor_size;
    uint32_t m_sampler_descriptor_size;

    wrl::ComPtr<ID3D12DescriptorHeap> m_RTV_heap; // TOOD: should I move RTV_heap to Window?
    wrl::ComPtr<ID3D12DescriptorHeap> m_DSV_heap;
    wrl::ComPtr<ID3D12DescriptorHeap> m_CBV_SRV_UAV_heap;
    wrl::ComPtr<ID3D12DescriptorHeap> m_sampler_heap;
};
} // namespace engine
