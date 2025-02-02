#include "model_manager.h"

#include <cassert>

namespace engine
{
ModelManager *ModelManager::s_instance = nullptr;

void ModelManager::Create()
{
    assert(s_instance == nullptr && "ModelManager instance already created\n");
    s_instance = new ModelManager;
}

ModelManager *ModelManager::GetInstance()
{
    assert(s_instance && "ModelManager instance is not created\n");
    return s_instance;
}

void ModelManager::Destroy()
{
    assert(s_instance && "ModelManager instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

std::shared_ptr<Mesh> ModelManager::GetOrCreateModel(
    const std::string &modelName,
    const std::wstring &pathToTexture,
    void *vertexBufferData,
    size_t vertexBufferSize,
    size_t vertexSize,
    void *indexBufferData,
    size_t indexBufferSize,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    auto found = m_models.find(modelName);
    if (found != m_models.end())
        return found->second;

    return m_models.try_emplace(
        modelName,
        std::make_shared<Mesh>(
            pathToTexture,
            vertexBufferData,
            vertexBufferSize,
            vertexSize,
            indexBufferData,
            indexBufferSize,
            position,
            scale,
            rotation)).first->second;
}

std::shared_ptr<Mesh> ModelManager::GenerateUnitCube(
    const std::string &modelName,
    const std::wstring &pathToTexture,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    struct VertexBufferGPU
    {
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT2 m_uv;
    };

    VertexBufferGPU vertices[] =
    {
        {
            { -1.0f, -1.0f, -1.0f },
            { 0.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, -1.0f },
            { 0.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f }
        },
        {
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 0.0f }
        },
        {
            { 1.0f, -1.0f, 1.0f },
            { 0.0f, 0.0f }
        },
        {
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f }
        },
    };

    uint16_t indices[] =
    {
        // front
        0, 1, 2,
        0, 2, 3,

        // right
        3, 2, 4,
        2, 7, 4,

        // back
        4, 6, 5,
        4, 7, 6,

        // left
        0, 6, 1,
        0, 5, 6,

        // top
        2, 1, 6,
        2, 6, 7,

        // bot
        3, 5, 0,
        3, 4, 5
    };

    return GetOrCreateModel(
        modelName,
        pathToTexture,
        vertices,
        sizeof(vertices),
        sizeof(VertexBufferGPU),
        indices,
        sizeof(indices),
        position,
        scale,
        rotation);
}
} // namespace engine