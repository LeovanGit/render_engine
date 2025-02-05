#include "shader_manager.h"

#include <cassert>

namespace engine
{
ShaderManager *ShaderManager::s_instance = nullptr;

void ShaderManager::Create()
{
    assert(s_instance == nullptr && "ShaderManager instance already created\n");
    s_instance = new ShaderManager;
}

ShaderManager *ShaderManager::GetInstance()
{
    assert(s_instance && "ShaderManager instance is not created\n");
    return s_instance;
}

void ShaderManager::Destroy()
{
    assert(s_instance && "ShaderManager instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

std::shared_ptr<Shader> ShaderManager::GetOrCreateShader(
    uint32_t shaderStages,
    const std::wstring &pathToFile,
    D3D11_INPUT_ELEMENT_DESC inputLayout[],
    size_t numElements,
    D3D11_SO_DECLARATION_ENTRY inputSignature[],
    size_t numSignatureElements)
{
    auto found = m_shaders.find(pathToFile);
    if (found != m_shaders.end())
        return found->second;

    return m_shaders.try_emplace(
        pathToFile,
        std::make_shared<Shader>(
            shaderStages,
            pathToFile,
            inputLayout,
            numElements,
            inputSignature,
            numSignatureElements)).first->second;
}
} // namespace engine