#include "pipeline_manager.h"

#include <cassert>

namespace engine
{
PipelineManager *PipelineManager::s_instance = nullptr;

void PipelineManager::Create()
{
    assert(s_instance == nullptr && "PipelineManager instance already created\n");
    s_instance = new PipelineManager;
}

PipelineManager *PipelineManager::GetInstance()
{
    assert(s_instance && "PipelineManager instance is not created\n");
    return s_instance;
}

void PipelineManager::Destroy()
{
    assert(s_instance && "PipelineManager instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

std::shared_ptr<Pipeline> PipelineManager::GetOrCreatePipeline(
    const std::string &name,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC *PSODesc)
{
    auto found = m_pipelines.find(name);
    if (found != m_pipelines.end())
        return found->second;

    return m_pipelines.try_emplace(name, std::make_shared<Pipeline>(PSODesc)).first->second;
}
} // namespace engine