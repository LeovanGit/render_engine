#pragma once

#include <iostream>
#include <unordered_map>

#include "pipeline.h"
#include "utils/utils.h"

namespace engine
{
class PipelineManager : public NonCopyable
{
public:
    static void Create();
    static PipelineManager *GetInstance();
    static void Destroy();

    std::shared_ptr<Pipeline> GetOrCreatePipeline(
        const std::string &name,
        D3D12_GRAPHICS_PIPELINE_STATE_DESC *PSODesc = nullptr);

    std::unordered_map<std::string, std::shared_ptr<Pipeline>> m_pipelines;

private:
    PipelineManager() = default;
    ~PipelineManager() = default;

    static PipelineManager *s_instance;
};
} // namespace engine
