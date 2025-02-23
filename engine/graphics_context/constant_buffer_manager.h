#pragma once

#include "utils/utils.h"

#include "buffer.h"

#include <DirectXMath.h>

namespace engine
{
/*class ConstantBufferManager : public NonCopyable
{
public:
    static void Create();
    static ConstantBufferManager *GetInstance();
    static void Destroy();

    struct PerView
    {
        DirectX::XMFLOAT4X4 viewProjMatrix;
        DirectX::XMFLOAT3 cameraPostionWS;
        float offset0;
        // camera frustum near plane's corners in WS:
        DirectX::XMFLOAT4 nearPlaneCornersWS[3];
    };

    struct PerMesh
    {
        DirectX::XMFLOAT4X4 modelMatrix;
    };

    void CreateConstantBuffers();

    void UpdatePerViewConstantBuffer(PerView &data);
    void UpdatePerMeshConstantBuffer(PerMesh &data);

    void BindPerViewConstantBuffer();
    void BindPerMeshConstantBuffer();

    std::shared_ptr<Buffer> m_perView;
    std::shared_ptr<Buffer> m_perMesh;

private:
    ConstantBufferManager();
    ~ConstantBufferManager() = default;

    static ConstantBufferManager *s_instance;
};*/
} // namespace engine
