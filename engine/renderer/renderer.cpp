#include "renderer.h"

namespace engine
{
Renderer::Renderer(std::shared_ptr<Window> window)
    : m_window(window)
    , m_camera(nullptr)
    , m_opaqueInstances(new OpaqueInstances)
    //, m_terrain(nullptr)
    //, m_skybox(nullptr)
    //, m_postprocess(nullptr)
    //, m_quadSphere(nullptr)
{
    CreateConstantBuffers();
}

void Renderer::CreateConstantBuffers()
{
    m_perViewConstantBuffer = std::make_shared<Buffer>(
        BufferUsage_ConstantBuffer,
        nullptr,
        sizeof(PerViewData));
}

void Renderer::UpdatePerViewConstantBuffer()
{
    PerViewData perViewData;
    DirectX::XMStoreFloat4x4(&perViewData.viewProjMatrix, m_camera->GetViewProjMatrix());
    DirectX::XMStoreFloat3(&perViewData.cameraPostionWS, m_camera->GetPosition());
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[0], m_camera->Reproject(-1.0f, 1.0f));
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[1], m_camera->Reproject(-1.0f, -1.0f));
    DirectX::XMStoreFloat4(&perViewData.nearPlaneCornersWS[2], m_camera->Reproject(1.0f, -1.0f));

    m_perViewConstantBuffer->Update(static_cast<void *>(&perViewData), sizeof(perViewData));
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

    UpdatePerViewConstantBuffer();

    globals->BindGlobalRootSignature();
    globals->BindConstantBuffers();
    globals->BindShaderResources();
    globals->BindSamplers();
    
    m_opaqueInstances->Render();

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

    //m_quadSphere = nullptr;
    //m_postprocess = nullptr;
    //m_skybox = nullptr;
    //m_terrain = nullptr;
    m_opaqueInstances = nullptr;
    m_camera = nullptr;
    m_window = nullptr;
}
} // namespace engine
