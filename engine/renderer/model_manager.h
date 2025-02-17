#pragma once

#include <iostream>
#include <unordered_map>

#include "mesh.h"

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
        const std::wstring &pathToTexture,
        void *vertexBufferData,
        size_t vertexBufferSize,
        size_t vertexSize,
        void *indexBufferData,
        size_t indexBufferSize,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    std::shared_ptr<Mesh> GeneratePlane(
        const std::string &modelName,
        const std::wstring &pathToTexture,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    std::shared_ptr<Mesh> GeneratePlanesGrid(
        const std::string &modelName,
        const std::wstring &pathToTexture,
        uint32_t rows,
        uint32_t cols,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    std::shared_ptr<Mesh> GenerateCube(
        const std::string &modelName,
        const std::wstring &pathToTexture,
        DirectX::XMFLOAT3 startPosition,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_models;

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager *s_instance;
};
} // engine