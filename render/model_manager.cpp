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

std::shared_ptr<VertexBuffer> ModelManager::GetOrCreateModel(
    const std::string & modelName,
    void *data,
    size_t bufferSize,
    size_t vertexSize)
{
    auto found = m_models.find(modelName);
    if (found != m_models.end())
        return found->second;

    return m_models.try_emplace(
        modelName,
        std::make_shared<VertexBuffer>(
            data,
            bufferSize,
            vertexSize)).first->second;
}
} // namespace engine