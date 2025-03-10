#include "shader.h"

#include <cassert>

#include <d3dcompiler.h>

namespace engine
{
std::string GetShaderTarget(ShaderStage type)
{
    switch (type)
    {
    case ShaderStage_VertexShader:
        return "vs_5_1";
    case ShaderStage_HullShader:
        return "hs_5_1";
    case ShaderStage_DomainShader:
        return "ds_5_1";
    case ShaderStage_GeometryShader:
        return "gs_5_1";
    case ShaderStage_PixelShader:
        return "ps_5_1";
    case ShaderStage_ComputeShader:
        return "cs_5_1";
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
    case ShaderStage_HullShader:
        return "mainHS";
    case ShaderStage_DomainShader:
        return "mainDS";
    case ShaderStage_GeometryShader:
        return "mainGS";
    case ShaderStage_PixelShader:
        return "mainPS";
    case ShaderStage_ComputeShader:
        return "mainCS";
    default:
        assert(false && "Unknown shader type\n");
        return "";
    };
}

Shader::Shader(uint32_t shaderStages,
    const std::wstring &pathToFile,
    D3D12_INPUT_LAYOUT_DESC inputLayout)
    : m_shaderStages(shaderStages)
    , m_pathToFile(pathToFile)
    , m_inputLayout(inputLayout)
    , m_VSBytecode(nullptr)
    , m_HSBytecode(nullptr)
    , m_DSBytecode(nullptr)
    , m_GSBytecode(nullptr)
    , m_PSBytecode(nullptr)
    , m_CSBytecode(nullptr)
{
    if (shaderStages & ShaderStage_VertexShader)
        Compile(ShaderStage::ShaderStage_VertexShader);

    if (shaderStages & ShaderStage_HullShader)
        Compile(ShaderStage::ShaderStage_HullShader);

    if (shaderStages & ShaderStage_DomainShader)
        Compile(ShaderStage::ShaderStage_DomainShader);

    if (shaderStages & ShaderStage_GeometryShader)
        Compile(ShaderStage::ShaderStage_GeometryShader);

    if (shaderStages & ShaderStage_PixelShader)
        Compile(ShaderStage::ShaderStage_PixelShader);

    if (shaderStages & ShaderStage_ComputeShader)
        Compile(ShaderStage::ShaderStage_ComputeShader);
}

void Shader::Compile(ShaderStage type)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG)
    uint32_t flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint32_t flags = 0;
#endif

    ID3DBlob **compiled = nullptr;
    ComPtr<ID3DBlob> error = nullptr;

    switch (type)
    {
    case ShaderStage_VertexShader:
        compiled = m_VSBytecode.GetAddressOf();
        break;
    case ShaderStage_HullShader:
        compiled = m_HSBytecode.GetAddressOf();
        break;
    case ShaderStage_DomainShader:
        compiled = m_DSBytecode.GetAddressOf();
        break;
    case ShaderStage_GeometryShader:
        compiled = m_GSBytecode.GetAddressOf();
        break;
    case ShaderStage_PixelShader:
        compiled = m_PSBytecode.GetAddressOf();
        break;
    case ShaderStage_ComputeShader:
        compiled = m_CSBytecode.GetAddressOf();
        break;
    default:
        assert(false && "Unknown shader type\n");
    }

    hr = D3DCompileFromFile(
        m_pathToFile.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        GetShaderEntryPoint(type).c_str(),
        GetShaderTarget(type).c_str(),
        flags,
        0,
        compiled,
        error.GetAddressOf());

    if (hr < 0)
    {
        if (error)
        {
            //std::wcout.write(static_cast<wchar_t *>(error->GetBufferPointer()), error->GetBufferSize());
            //std::wcout << std::endl;
            
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", static_cast<char *>(error->GetBufferPointer()));
        }

        assert(false && "Failed to compile shader\n");
    }
}
} // namespace engine
