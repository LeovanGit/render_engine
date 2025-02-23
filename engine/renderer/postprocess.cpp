#include "postprocess.h"

namespace engine
{
/*PostProcess::PostProcess()
    : m_shader(nullptr)
    , m_RWTexture(nullptr)
    , m_readBuffer(nullptr)
    , m_readStructuredBuffer(nullptr)
    , m_RWBuffer(nullptr)
    , m_RWStructuredBuffer(nullptr)
{
    ShaderManager *sm = ShaderManager::GetInstance();
    m_shader = sm->GetOrCreateShader(ShaderStage_ComputeShader, L"../assets/shaders/postprocess.hlsl");

    TextureManager *tm = TextureManager::GetInstance();
    m_RWTexture = tm->GetOrCreateTexture(L"RWTexture1000x1000", 1000, 1000, TextureUsage_UAV);

    uint32_t readBufferData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    
    m_readBuffer = std::make_shared<Buffer>(
        readBufferData,
        sizeof(readBufferData),
        sizeof(uint32_t),
        BufferUsage_ReadBuffer);

    struct StructBufferData
    {
        float x;
        float y;
    };
    StructBufferData structBufferData[] = { { 1, 1 }, { 2, 2 }, { 3, 3 } };
    
    m_readStructuredBuffer = std::make_shared<Buffer>(
        structBufferData,
        sizeof(structBufferData),
        sizeof(StructBufferData),
        BufferUsage_ReadStructuredBuffer);

    m_RWBuffer = std::make_shared<Buffer>(
        nullptr,
        2 * sizeof(uint32_t),
        sizeof(uint32_t),
        BufferUsage_RWBuffer);

    m_RWStructuredBuffer = std::make_shared<Buffer>(
        nullptr,
        1000 * sizeof(StructBufferData),
        sizeof(StructBufferData),
        BufferUsage_RWStructuredBuffer);
}

void PostProcess::Render()
{
    Globals *globals = Globals::GetInstance();

    m_shader->Bind();
    m_RWTexture->Bind(0, TextureUsage_UAV, ShaderStage_ComputeShader);

    m_readBuffer->Bind(0, ShaderStage_ComputeShader);
    m_readStructuredBuffer->Bind(1, ShaderStage_ComputeShader);
    m_RWBuffer->Bind(1, ShaderStage_ComputeShader);
    m_RWStructuredBuffer->Bind(2, ShaderStage_ComputeShader);

    globals->m_deviceContext->Dispatch(m_RWTexture->m_width, m_RWTexture->m_height, 1);

    m_RWStructuredBuffer->Unbind();
    m_RWBuffer->Unbind();
    m_readStructuredBuffer->Unbind();
    m_readBuffer->Unbind();

    m_RWTexture->Unbind();
    m_shader->Unbind();
}

PostProcess::~PostProcess()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_RWStructuredBuffer = nullptr;
    m_RWBuffer = nullptr;
    m_readStructuredBuffer = nullptr;
    m_readBuffer = nullptr;
    m_RWTexture = nullptr;
    m_shader = nullptr;
}*/
} // namespace engine