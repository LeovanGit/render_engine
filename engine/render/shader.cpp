#include "shader.h"

namespace engine
{
// looks like path relative project.vcxproj
Shader::Shader(const std::wstring &path,
               const std::string &entry_point,
               const std::string &shader_model) :
    m_path(path),
    m_entry_point(entry_point),
    m_shader_model(shader_model),
    m_compiled_shader(nullptr)
{
    CompileShader();
}

void Shader::CompileShader()
{
#if defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG)
    UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT flags = 0;
#endif

    wrl::ComPtr<ID3DBlob> error_msg = nullptr;

    HRESULT result = D3DCompileFromFile(
        m_path.c_str(),
        nullptr, // const D3D_SHADER_MACRO * pDefines
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        m_entry_point.c_str(),
        m_shader_model.c_str(),
        flags,
        0,
        m_compiled_shader.GetAddressOf(),
        &error_msg);

    if (FAILED(result))
    {
        if (error_msg)
        {
            std::wcout << "SHADER COMPILATION ERROR:\n"
                       << m_path << "\n"
                       << static_cast<char *>(error_msg->GetBufferPointer()) << "\n\n";
        }
        else
        {
            std::wcout << "SHADER NOT FOUND\n" << m_path << "\n\n";
        }
        
        assert("D3DCompileFromFile()");
    }
}

void Shader::Bind()
{
    // bind shader to pipeline here
}
} // namespace engine
