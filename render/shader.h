#pragma once

#include <iostream>

#include <SDL.h>

#include "utils/d3d11common.h"
#include "render/globals.h"

namespace engine
{
enum ShaderStage
{
    ShaderStage_VertexShader = (1 << 0),
    ShaderStage_PixelShader = (1 << 1)
};

std::string GetShaderTarget(ShaderStage type);

class Shader
{
public:
    Shader(uint32_t shaderStages,
        const std::wstring &pathToFile,
        D3D11_INPUT_ELEMENT_DESC inputLayout[] = nullptr,
        size_t numElements = 0);

    ~Shader() = default;

private:
    void Compile(ShaderStage type,
        D3D11_INPUT_ELEMENT_DESC inputLayout[] = nullptr,
        size_t numElements = 0);

public:
    void Bind();

    uint32_t m_shaderStages;
    std::wstring m_pathToFile;

    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
};
} // namespace engine