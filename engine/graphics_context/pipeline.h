#pragma once

#include "utils/d3dcommon.h"

namespace engine
{
class Pipeline
{
public:
    Pipeline(D3D12_GRAPHICS_PIPELINE_STATE_DESC *PSODesc = nullptr);
    ~Pipeline() = default;

    void Bind();

    ComPtr<ID3D12PipelineState> m_PSO;
};
} // namespace engine