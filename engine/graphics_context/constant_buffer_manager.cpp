#include "constant_buffer_manager.h"

#include <cassert>

namespace engine
{
ConstantBufferManager *ConstantBufferManager::s_instance = nullptr;

void ConstantBufferManager::Create()
{
    assert(s_instance == nullptr && "ConstantBufferManager instance already created\n");
    s_instance = new ConstantBufferManager;
}

ConstantBufferManager *ConstantBufferManager::GetInstance()
{
    assert(s_instance && "ConstantBufferManager instance is not created\n");
    return s_instance;
}

void ConstantBufferManager::Destroy()
{
    assert(s_instance && "ConstantBufferManager instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

ConstantBufferManager::ConstantBufferManager()
    : m_perView(nullptr)
    , m_perMesh(nullptr)
{
    CreateConstantBuffers();
}

void ConstantBufferManager::CreateConstantBuffers()
{
    m_perView = std::make_shared<Buffer>(
        nullptr,
        sizeof(PerView),
        sizeof(PerView),
        BufferUsage_ConstantBuffer);

    //m_perMesh = std::make_shared<Buffer>(
    //    nullptr,
    //    sizeof(PerMesh),
    //    sizeof(PerMesh),
    //    BufferUsage_ConstantBuffer);
}

void ConstantBufferManager::UpdatePerViewConstantBuffer(PerView &data)
{
    m_perView->Update(static_cast<void *>(&data), sizeof(data));
}

void ConstantBufferManager::UpdatePerMeshConstantBuffer(PerMesh &data)
{
    m_perMesh->Update(static_cast<void *>(&data), sizeof(data));
}

void ConstantBufferManager::BindPerViewConstantBuffer()
{
    m_perView->Bind(0, ShaderStage::ShaderStage_AllExceptCompute);
}

void ConstantBufferManager::BindPerMeshConstantBuffer()
{
    m_perMesh->Bind(1, ShaderStage::ShaderStage_AllExceptCompute);
}
} // namespace engine
