#include "renderer.h"

#include "graphics_context/constant_buffer_manager.h"

namespace engine
{
Renderer::Renderer(std::shared_ptr<Window> window)
    : m_window(window)
    //, m_opaqueInstances(new OpaqueInstances)
    //, m_terrain(nullptr)
    //, m_skybox(nullptr)
    //, m_postprocess(nullptr)
    //, m_quadSphere(nullptr)
{
    Globals *globals = Globals::GetInstance();

    globals->BindSamplers(); // bind all samplers at the begin
}

void Renderer::Render(bool debugMode)
{
    Globals *globals = Globals::GetInstance();
    
    globals->BeginCommandsRecording();

    // viewport and scissor rect needs to be reset whenever the command list is reset:
    m_window->SetViewport();
    m_window->SetScissorRect();
    
    m_window->ClearRenderTarget();
    m_window->BindRenderTarget();

    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();
    ConstantBufferManager::PerView perViewData;
    DirectX::XMStoreFloat4x4(&perViewData.viewProjMatrix, m_camera->GetViewProjMatrix());
    DirectX::XMStoreFloat3(&perViewData.cameraPostionWS, m_camera->GetPosition());
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[0], m_camera->Reproject(-1.0f, 1.0f));
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[1], m_camera->Reproject(-1.0f, -1.0f));
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[2], m_camera->Reproject(1.0f, -1.0f));

    cbm->UpdatePerViewConstantBuffer(perViewData);

    globals->BindRootSignature();
    globals->BindCBVDescriptorsHeap();
    globals->BindCBV();
    
    PipelineManager *pm = PipelineManager::GetInstance();
    pm->GetOrCreatePipeline("opaque")->Bind();

    m_mesh->m_vertexBuffer->Bind(0);
    m_instanceBuffer->Bind(1);
    m_mesh->m_indexBuffer->Bind();

    globals->m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->m_commandList->DrawIndexedInstanced(
        m_mesh->m_indexBuffer->m_byteSize / m_mesh->m_indexBuffer->m_stride,
        1,
        0,
        0,
        0);

    globals->EndCommandsRecording();
    globals->Submit();
    globals->FlushCommandQueue();

    m_window->Present();

    




    /*

    cbm->BindPerMeshConstantBuffer();

    m_window->SetViewport();

    m_window->ClearRenderTarget();
    m_window->BindRenderTarget(); // RTV + DSV

    // bind states which will not change during frame rendering:
    globals->BindDepthStencilState();
    globals->BindRasterizerState();

    m_opaqueInstances->Render();
    if (debugMode) m_opaqueInstances->RenderDebug();
    m_quadSphere->Render();
    m_terrain->Render();
    m_skybox->Render();
    m_postprocess->Render();

    m_window->Present();*/
}

void Renderer::Destroy()
{
    Globals *globals = Globals::GetInstance();
    
    // unbind all:
    //globals->m_deviceContext->ClearState();

    //m_quadSphere = nullptr;
    //m_postprocess = nullptr;
    //m_skybox = nullptr;
    //m_terrain = nullptr;
    //m_opaqueInstances = nullptr;
    m_camera = nullptr;
    m_window = nullptr;
}
} // namespace engine
