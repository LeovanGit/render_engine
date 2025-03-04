#pragma once

#include <iostream>

#include <SDL.h>

#include "utils/d3dcommon.h"
#include "globals.h"

namespace engine
{
std::string GetShaderTarget(ShaderStage type);
std::string GetShaderEntryPoint(ShaderStage type);

class Shader
{
public:
    Shader(uint32_t shaderStages,
        const std::wstring &pathToFile,
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = nullptr,
        size_t numElements = 0);

    ~Shader() = default;

private:
    void Compile(ShaderStage type);

public:
    void Bind();

    void Unbind();

    uint32_t m_shaderStages;
    std::wstring m_pathToFile;

    D3D12_INPUT_LAYOUT_DESC m_inputLayout;

    ComPtr<ID3DBlob> m_VSBytecode;
    ComPtr<ID3DBlob> m_HSBytecode;
    ComPtr<ID3DBlob> m_DSBytecode;
    ComPtr<ID3DBlob> m_GSBytecode;
    ComPtr<ID3DBlob> m_PSBytecode;

    ComPtr<ID3DBlob> m_CSBytecode;
};
} // namespace engine
