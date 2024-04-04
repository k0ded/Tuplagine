
cbuffer constants : register(b0)
{
    float4x4 transform;
    float3   lightvector;
}

cbuffer CameraData : register(b1)
{
    float4x4 worldToClip;
    float4x4 worldToCamera;
}

struct VertexData
{
    float3 position : POS;
    float3 normal   : NOR;
    float2 texcoord : TEX;
    float3 color    : COL;
};

struct PixelData
{
    float4 position : SV_POSITION;
    float2 texcoord : TEX;
    float4 color    : COL;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

PixelData vert(VertexData vertex)
{
    float light = clamp(dot(mul(vertex.normal, transform), normalize(-lightvector)), 0.0f, 1.0f) * 0.8f + 0.2f;

    PixelData output;

    output.position = mul(mul(worldToClip, transform), float4(vertex.position, 1.0f));
    output.texcoord = vertex.texcoord;
    output.color = float4(vertex.color * light, 1.0f);

    return output;
}

float4 frag(PixelData pixel) : SV_TARGET
{
    return mytexture.Sample(mysampler, pixel.texcoord) * pixel.color;
}