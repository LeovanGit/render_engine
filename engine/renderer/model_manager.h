#pragma once

#include <iostream>
#include <unordered_map>

#include "utils/utils.h"
#include "model.h"

#include <DirectXMath.h>

namespace engine
{
class ModelManager : public NonCopyable
{
public:
    struct VertexData
    {
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT2 m_uv;
    };

    static void Create();
    static ModelManager *GetInstance();
    static void Destroy();

    std::shared_ptr<Mesh> GetOrCreateModel(
        const std::string &modelName,
        void *vertexBufferData,
        size_t vertexBufferSize,
        size_t vertexSize,
        void *indexBufferData,
        size_t indexBufferSize);

    std::shared_ptr<Mesh> GenerateUnitPlane(const std::string &modelName);

    std::shared_ptr<Mesh> GeneratePlanesUnitGrid(
        const std::string &modelName,
        uint32_t rows,
        uint32_t cols);

    std::shared_ptr<Mesh> GenerateUnitCube(const std::string &modelName);

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_models;

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager *s_instance;
};
} // engine
