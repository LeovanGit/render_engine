#pragma once

#include <iostream>
#include <unordered_map>

#include "vertex_buffer.h"

namespace engine
{
class ModelManager : public NonCopyable
{
public:
    static void Create();
    static ModelManager *GetInstance();
    static void Destroy();

    std::shared_ptr<VertexBuffer> GetOrCreateModel(
        const std::string & modelName,
        void *data,
        size_t bufferSize,
        size_t vertexSize);

    std::unordered_map<std::string, std::shared_ptr<VertexBuffer>> m_models;

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager *s_instance;
};
} // engine