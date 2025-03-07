#include "pipeline.h"

#include "globals.h"

#include <cassert>

namespace engine
{
Pipeline::Pipeline(D3D12_GRAPHICS_PIPELINE_STATE_DESC *PSODesc)
{
    Globals *globals = Globals::GetInstance();

    HRESULT hr = globals->m_device->CreateGraphicsPipelineState(PSODesc, IID_PPV_ARGS(&m_PSO));
    assert(hr >= 0 && "Failed to create PSO\n");
}

void Pipeline::Bind()
{
    Globals *globals = Globals::GetInstance();

    globals->m_commandList->SetPipelineState(m_PSO.Get());
}
} // namespace engine