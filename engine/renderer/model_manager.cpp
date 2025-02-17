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

std::shared_ptr<Mesh> ModelManager::GeneratePlane(
    const std::string &modelName,
    const std::wstring &pathToTexture,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    VertexData vertices[] =
    {
        {
            { -1.0f, -1.0f, 0.0f },
            { 0.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, 0.0f },
            { 0.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f, 0.0f },
            { 1.0f, 1.0f }
        },
        {
            { 1.0f, -1.0f, 0.0f },
            { 1.0f, 0.0f }
        },
    };

    uint16_t indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    return GetOrCreateModel(
        modelName,
        pathToTexture,
        vertices,
        sizeof(vertices),
        sizeof(VertexData),
        indices,
        sizeof(indices),
        position,
        scale,
        rotation);
}

std::shared_ptr<Mesh> ModelManager::GeneratePlanesGrid(
    const std::string &modelName,
    const std::wstring &pathToTexture,
    uint32_t rows,
    uint32_t cols,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    std::vector<VertexData> vertices;
    std::vector<uint16_t> indices;
    
    // 2x2 planes grid is 3x3 vertices grid:
    uint32_t vertexRows = rows + 1;
    uint32_t vertexCols = cols + 1;

    // all planes are unit size (1x1),
    // after generation transform matrix will scale, rotate and translate entire grid
    float planeWidth = 1.0f;
    float planeHeight = 1.0f;
    
    // left-top corner (grid's center is (0, 0, 0)):
    DirectX::XMFLOAT3 startPosition;
    startPosition.x = -0.5f * (cols * planeWidth);
    startPosition.y = 0.5f * (rows * planeHeight);
    startPosition.z = 0.0f;

    // add only unique vertices:
    for (uint32_t row = 0; row != vertexRows; ++row)
    {
        for (uint32_t col = 0; col != vertexCols; ++col)
        {
            VertexData vertex;
            vertex.m_position.x = startPosition.x + col * planeWidth;
            vertex.m_position.y = startPosition.y - row * planeHeight;
            vertex.m_position.z = startPosition.z;

            // uv starts from bottom-left corner and should be in [0; 1] range:
            vertex.m_uv.x = (1.0f / cols) * col;
            vertex.m_uv.y = 1.0f - (1.0f / rows) * row; // inverse

            vertices.push_back(vertex);
        }
    }

    // create index buffer from unique vertices
    // pay attention: vertices was added row-by-row in prev. cycle
    for (uint32_t row = 0; row != rows; ++row)
    {
        for (uint32_t col = 0; col != cols; ++col)
        {
            // each iteration is a quad (2 triangles)
            uint16_t leftTopIndex = vertexCols * row + col;
            uint16_t rightTopIndex = leftTopIndex + 1;
            uint16_t leftBottomIndex = vertexCols * (row + 1) + col;
            uint16_t rightBottomIndex = leftBottomIndex + 1;

            indices.push_back(leftTopIndex);
            indices.push_back(rightTopIndex);
            indices.push_back(leftBottomIndex);

            indices.push_back(leftBottomIndex);
            indices.push_back(rightTopIndex);
            indices.push_back(rightBottomIndex);
        }
    }

    return GetOrCreateModel(
        modelName,
        pathToTexture,
        vertices.data(),
        vertices.size() * sizeof(VertexData),
        sizeof(VertexData),
        indices.data(),
        indices.size() * sizeof(uint16_t),
        position,
        scale,
        rotation);
}

std::shared_ptr<Mesh> ModelManager::GenerateCube(
    const std::string &modelName,
    const std::wstring &pathToTexture,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    VertexData vertices[] =
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
        sizeof(VertexData),
        indices,
        sizeof(indices),
        position,
        scale,
        rotation);
}
} // namespace engine