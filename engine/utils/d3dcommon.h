#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#include <wrl\client.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

enum ShaderStage
{
    ShaderStage_VertexShader = (1 << 0),
    ShaderStage_HullShader = (1 << 1),
    ShaderStage_DomainShader = (1 << 2),
    ShaderStage_GeometryShader = (1 << 3),
    ShaderStage_PixelShader = (1 << 4),
    ShaderStage_ComputeShader = (1 << 5),

    ShaderStage_Count = 6
};

enum RootSignatureSlot
{
    RootSignatureSlot_CBV = 0,
    RootSignatureSlot_SRV = 1,
    RootSignatureSlot_Sampler = 2,

    RootSignatureSlot_Count = 3
};

template<class T>
T AlignUpToMultipleOf256(T value)
{
    static_assert(std::is_integral_v<T> && "Type T is not integer");

    return (value + 255) & ~255;
}
