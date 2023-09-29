#pragma once

#include <memory>
#include <cassert>

#include "../source/non_copyable.h"
#include "d3d.h"

namespace engine
{
struct Settings
{
    DXGI_FORMAT backbuffer_format;
    uint8_t swapchain_buffer_count;
};

class Engine final : NonCopyable
{
public:
    static void Init();

    static const Engine *GetInstance();

    static void Deinit();
    
private:
    Engine() = default;
    ~Engine() = default;

    void LoadEngineSettings();

    static Engine *m_instance;

public:
    Settings settings;
};
} // namespace engine
