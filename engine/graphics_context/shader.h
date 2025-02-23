#pragma once

#include <iostream>

#include <SDL.h>

#include "utils/d3dcommon.h"
#include "globals.h"

namespace engine
{
/*std::string GetShaderTarget(ShaderStage type);
std::string GetShaderEntryPoint(ShaderStage type);

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

    void Unbind();

    uint32_t m_shaderStages;
    std::wstring m_pathToFile;

    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11HullShader> m_hullShader;
    ComPtr<ID3D11DomainShader> m_domainShader;
    ComPtr<ID3D11GeometryShader> m_geometryShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;

    ComPtr<ID3D11ComputeShader> m_computeShader;
};*/
} // namespace engine