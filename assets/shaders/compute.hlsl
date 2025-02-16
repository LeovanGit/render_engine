#include "globals.hlsli"

RWTexture2D<float4> outputTexture : register(u0);

Buffer<uint> readBuffer : register(t0);

struct BufferData
{
    float value0;
    float value1;
};

StructuredBuffer<BufferData> readStructuredBuffer : register(t1);

[numthreads(8, 8, 1)]
void mainCS(uint3 threadID : SV_DispatchThreadID,
            uint3 groupID : SV_GroupID)
{
    float width;
    float height;
    outputTexture.GetDimensions(width, height);

    if (threadID.x > width || threadID.y > height) return;

    float r = (float)groupID.x / readBuffer[4] / readStructuredBuffer[1].value0;
    float g = (float)groupID.y / readBuffer[4];

    outputTexture[threadID.xy] = float4(r, g, 0.0f, 1.0f);
}
