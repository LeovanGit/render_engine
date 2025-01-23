#include "shader_manager.h"

#include <cassert>
#include <iostream>

namespace engine
{
std::string GetShaderTarget(Shader::ShaderType type)
{
    switch (type)
    {
    case Shader::ShaderType::VertexShader:
        return "vs_5_0";
    case Shader::ShaderType::PixelShader:
        return "ps_5_0";
    default:
        assert(false && "Unknown shader type\n");
    };
}

Shader::Shader(Shader::ShaderType type,
               const std::wstring &pathToFile,
               const std::string &entryPoint,
               D3D11_INPUT_ELEMENT_DESC inputLayout[],
               size_t numElements)
    : m_type(type)
    , m_pathToFile(pathToFile)
    , m_entryPoint(entryPoint)
    , m_vertexShader(nullptr)
    , m_pixelShader(nullptr)
    , m_inputLayout(nullptr)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG)
    uint32_t flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint32_t flags = 0;
#endif

    ComPtr<ID3DBlob> compiled; // bytecode
    ComPtr<ID3DBlob> error;

    hr = D3DCompileFromFile(
        m_pathToFile.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        m_entryPoint.c_str(),
        GetShaderTarget(m_type).c_str(),
        flags,
        0,
        &compiled,
        &error);

    if (hr < 0)
    {
        if (error)
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", static_cast<char*>(error->GetBufferPointer()));

        assert(false && "Failed to compile shader\n");
    }

    if (m_type == ShaderType::VertexShader)
    {
        hr = globals->m_device->CreateVertexShader(
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            nullptr,
            &m_vertexShader);
        assert(hr >= 0 && "Failed to create vertex shader\n");
    }
    else
    {
        hr = globals->m_device->CreatePixelShader(
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            nullptr,
            &m_pixelShader);
        assert(hr >= 0 && "Failed to create pixel shader\n");
    }

    if (m_type == ShaderType::VertexShader && inputLayout)
    {
        hr = globals->m_device->CreateInputLayout(
            inputLayout,
            numElements,
            compiled->GetBufferPointer(),
            compiled->GetBufferSize(),
            &m_inputLayout);
        assert(hr >= 0 && "Failed to create input layout\n");
    }
}

void Shader::Bind()
{
    Globals *globals = Globals::GetInstance();

    if (m_type == ShaderType::VertexShader)
    {
        globals->m_deviceContext->IASetInputLayout(m_inputLayout.Get());

        globals->m_deviceContext->VSSetShader(
            m_vertexShader.Get(),
            nullptr,
            0);
    }
    else
    {
        globals->m_deviceContext->PSSetShader(
            m_pixelShader.Get(),
            nullptr,
            0);
    }
}
} // namespace engine