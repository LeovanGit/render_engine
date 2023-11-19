#pragma once

#include <cassert>
#include <string>
#include <unordered_map>

#include "../source/non_copyable.h"
#include "shader.h"

namespace engine
{
class ShaderManager final : public NonCopyable
{
public:
    static void Init();

    static ShaderManager *GetInstance();

    static void Deinit();

    // fill all parameters except path with empty default value
    // so we can search shader in unordered_map using only its path
    Shader *GetShader(const std::string &path,
                      const std::string &entry_point = "",
                      const std::string &shader_model = "");

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager *m_instance;

    std::unordered_map<std::string, Shader> m_shaders;
};
} // namespace engine
