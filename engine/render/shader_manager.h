#pragma once

#include <string>

#include <SDL.h>

#include "utils/d3d11common.h"
#include "render/globals.h"

namespace engine
{
class Shader
{
public:
    enum class ShaderType
    {
        VertexShader = 0,
        PixelShader
    };

    Shader(Shader::ShaderType type,
           const std::wstring &pathToFile,
           const std::string &entryPoint,
           D3D11_INPUT_ELEMENT_DESC inputLayout[] = nullptr,
           size_t numElements = 0);

    void Bind();

    ShaderType m_type;
    std::wstring m_pathToFile;
    std::string m_entryPoint;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;

    ComPtr<ID3D11InputLayout> m_inputLayout;
};

std::string GetShaderTarget(Shader::ShaderType type);
} // engine