#pragma once

#include "utils/utils.h"

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