struct PerView
{
    float4x4 viewProjMatrix;
    float3 cameraPostionWS;
    float offset;

    // [0]: top-left
    // [1]: bottom-left
    // [2]: bottom-right
    float4 nearPlaneCornersWS[3];
};
ConstantBuffer<PerView> g_perView : register(b0, space0);

SamplerState sampler0 : register(s0, space0);
