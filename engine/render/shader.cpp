#include "shader.h"

#include <cassert>

namespace engine
{
std::string GetShaderTarget(ShaderStage type)
{
    switch (type)
    {
    case ShaderStage_VertexShader:
        return "vs_5_0";
    case ShaderStage_GeometryShader:
        return "gs_5_0";
    case ShaderStage_PixelShader:
        return "ps_5_0";
    default:
        assert(false && "Unknown shader type\n");
        return "";
    };
}

std::string GetShaderEntryPoint(ShaderStage type)
{
    switch (type)
    {
    case ShaderStage_VertexShader:
        return "mainVS";
    case ShaderStage_GeometryShader:
        return "mainGS";
    case ShaderStage_PixelShader:
        return "mainPS";
    default:
        assert(false && "Unknown shader type\n");
        return "";
    };
}

Shader::Shader(uint32_t shaderStages,
    const std::wstring &pathToFile,
    D3D11_INPUT_ELEMENT_DESC inputLayout[],
    size_t numElements)
    : m_shaderStages(shaderStages)
    , m_pathToFile(pathToFile)
    , m_inputLayout(nullptr)
    , m_vertexShader(nullptr)
    , m_pixelShader(nullptr)
{
    if (shaderStages & ShaderStage_VertexShader)
        Compile(ShaderStage_VertexShader, inputLayout, numElements);

    if (shaderStages & ShaderStage_GeometryShader)
        Compile(ShaderStage_GeometryShader);

    if (shaderStages & ShaderStage_PixelShader)
        Compile(ShaderStage_PixelShader);
}

void Shader::Compile(ShaderStage type,
    D3D11_INPUT_ELEMENT_DESC inputLayout[],
    size_t numElements)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG)
    uint32_t flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint32_t flags = 0;
#endif

    ComPtr<ID3DBlob> compiled = nullptr; // bytecode
    ComPtr<ID3DBlob> error = nullptr;

    hr = D3DCompileFromFile(
        m_pathToFile.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        GetShaderEntryPoint(type).c_str(),
        GetShaderTarget(type).c_str(),
        flags,
        0,
        compiled.GetAddressOf(),
        error.GetAddressOf());

    if (hr < 0)
    {
        if (error)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", static_cast<char *>(error->GetBufferPointer()));
        }

        assert(false && "Failed to compile shader\n");
    }

    if (type & ShaderStage_VertexShader)
    {
        hr = globals->m_device->CreateVertexShader(
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            nullptr,
            m_vertexShader.GetAddressOf());
        assert(hr >= 0 && "Failed to create vertex shader\n");
    }
    else if (type & ShaderStage_GeometryShader)
    {
        hr = globals->m_device->CreateGeometryShader(
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            nullptr,
            m_geometryShader.GetAddressOf());
        assert(hr >= 0 && "Failed to create geometry shader\n");
    }
    else if (type & ShaderStage_PixelShader)
    {
        hr = globals->m_device->CreatePixelShader(
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            nullptr,
            m_pixelShader.GetAddressOf());
        assert(hr >= 0 && "Failed to create pixel shader\n");
    }

    if (type & ShaderStage_VertexShader && inputLayout)
    {
        hr = globals->m_device->CreateInputLayout(
            inputLayout,
            static_cast<UINT>(numElements),
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            m_inputLayout.GetAddressOf());
        assert(hr >= 0 && "Failed to create input layout\n");
    }
}

void Shader::Bind()
{
    Globals *globals = Globals::GetInstance();

    if (m_shaderStages & ShaderStage_VertexShader)
    {
        globals->m_deviceContext->IASetInputLayout(m_inputLayout.Get());

        globals->m_deviceContext->VSSetShader(
            m_vertexShader.Get(),
            nullptr,
            0);
    }

    if (m_shaderStages & ShaderStage_GeometryShader)
    {
        globals->m_deviceContext->GSSetShader(
            m_geometryShader.Get(),
            nullptr,
            0);
    }

    if (m_shaderStages & ShaderStage_PixelShader)
    {
        globals->m_deviceContext->PSSetShader(
            m_pixelShader.Get(),
            nullptr,
            0);
    }
}
} // namespace engine