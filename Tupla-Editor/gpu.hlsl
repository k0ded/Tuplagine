cbuffer constants : register(b0)
{
    float4x4 transform;
    float4x4 projection;
    float3   lightvector;
}

struct vertexdata
{
    float3 position : POS;
    float3 normal   : NOR;
    float2 texcoord : TEX;
    float3 color    : COL;
};

struct pixeldata
{
    float4 position : SV_POSITION;
    float2 texcoord : TEX;
    float4 color    : COL;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

pixeldata vert(vertexdata vertex)
{
    float light = clamp(dot(mul(vertex.normal, transform), normalize(-lightvector)), 0.0f, 1.0f) * 0.8f + 0.2f;

    pixeldata output;

    output.position = mul(float4(vertex.position, 1.0f), mul(projection, transform));
    output.texcoord = vertex.texcoord;
    output.color = float4(vertex.color * light, 1.0f);

    return output;
}

float4 frag(pixeldata pixel) : SV_TARGET
{
    return mytexture.Sample(mysampler, pixel.texcoord) * pixel.color;
}