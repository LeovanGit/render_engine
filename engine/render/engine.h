#pragma once

#include <memory>
#include <cassert>

#include "../source/non_copyable.h"
#include "d3d.h"

namespace engine
{
class Engine final : NonCopyable
{
public:
    static void Init();

    static const Engine *GetInstance();

    static void Deinit();
    
private:
    Engine() = default;
    ~Engine() = default;

    static Engine *m_instance;
};
} // namespace engine
