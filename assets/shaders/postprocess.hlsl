struct BufferData
{
    float x;
    float y;
};

Buffer<uint> buffer0 : register(t0);
StructuredBuffer<BufferData> buffer1 : register(t1);

RWTexture2D<float4> texture0 : register(u0);
RWBuffer<uint> buffer2 : register(u1);
RWStructuredBuffer<BufferData> buffer3 : register(u2);

[numthreads(8, 8, 1)]
void mainCS(uint3 globalThreadID : SV_DispatchThreadID,
            uint3 localThreadID : SV_GroupThreadID,
            uint3 workGroupID : SV_GroupID)
{
    uint width = 0;
    uint height = 0;
    texture0.GetDimensions(width, height);

    if (globalThreadID.x > width || globalThreadID.y > height) return;

    // draw workgroups grid on texture:
    if (localThreadID.x == 0 || localThreadID.x == 8 ||
        localThreadID.y == 0 || localThreadID.y == 8)
    {
        texture0[globalThreadID.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
        return; 
    }

    // 125 8x8 workgroups in 1000x1000 texture:
    float r = float(workGroupID.x) / 125;
    float g = float(workGroupID.y) / 125;
    float b = float(workGroupID.z);

    texture0[globalThreadID.xy] = float4(r, g, b, 1.0f);

    // some usage of buffers:
    buffer2[globalThreadID.x] = float(buffer0[0]);

    BufferData data;
    data = buffer1[1];

    data.x *= globalThreadID.x;
    data.y *= globalThreadID.y;

    buffer3[globalThreadID.x] = data;
}
