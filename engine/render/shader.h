#pragma once

#include <iostream>
#include <string>
#include <cassert>

#include <d3dcompiler.h>

#include "../source/stdafx.h"

namespace engine
{
class Shader
{
public:
    Shader(const std::wstring &path,
           const std::string &entry_point,
           const std::string &shader_model);

    void Bind();

    std::wstring m_path;
    std::string m_entry_point;
    std::string m_shader_model;

    wrl::ComPtr<ID3DBlob> m_compiled_shader;

private:
    void CompileShader();
};
} // namespace engine
