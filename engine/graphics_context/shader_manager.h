#pragma once

#include <iostream>
#include <unordered_map>

#include "shader.h"

namespace engine
{
class ShaderManager : public NonCopyable
{
public:
    static void Create();
    static ShaderManager *GetInstance();
    static void Destroy();

    std::shared_ptr<Shader> GetOrCreateShader(
        uint32_t shaderStages,
        const std::wstring &pathToFile,
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = nullptr,
        size_t numElements = 0);
    
    std::unordered_map<std::wstring, std::shared_ptr<Shader>> m_shaders;

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager *s_instance;
};
} // namespace engine
