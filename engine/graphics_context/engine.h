#pragma once

#include "utils/utils.h"
#include "globals.h"
#include "shader_manager.h"
#include "renderer/model_manager.h"
#include "texture_manager.h"
#include "pipeline_manager.h"

namespace engine
{
class Engine : public NonCopyable
{
public:
    static void Create();
    static Engine *GetInstance();
    static void Destroy();

    void Init();
    void Deinit();

private:
    Engine() = default;
    ~Engine() = default;

    static Engine *s_instance;
};
} // namespace engine
