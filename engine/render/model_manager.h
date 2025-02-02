#pragma once

#include <iostream>
#include <unordered_map>

#include "model.h"

namespace engine
{
class ModelManager : public NonCopyable
{
public:
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

    std::shared_ptr<Mesh> GenerateUnitCube(
        const std::string &modelName,
        const std::wstring &pathToTexture,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_models;

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager *s_instance;
};
} // engine