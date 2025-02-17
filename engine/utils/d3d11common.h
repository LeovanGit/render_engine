#pragma once

#include <d3d11.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <d3dcompiler.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

enum ShaderStage
{
    ShaderStage_None = 0,
    ShaderStage_VertexShader = (1 << 0),
    ShaderStage_HullShader = (1 << 1),
    ShaderStage_DomainShader = (1 << 2),
    ShaderStage_GeometryShader = (1 << 3),
    ShaderStage_PixelShader = (1 << 4),
    ShaderStage_AllStages = (ShaderStage_VertexShader |
                             ShaderStage_HullShader |
                             ShaderStage_DomainShader |
                             ShaderStage_GeometryShader |
                             ShaderStage_PixelShader)
};
