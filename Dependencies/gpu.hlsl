
cbuffer PerObject : register(b0)
{
    float4x4 PO_transform;
    float3 PO_lightDirection;
}

cbuffer PerFrame : register(b1)
{
    float4x4 PF_worldToClip;
    float4x4 PF_worldToCamera;
}

struct VertexData
{
    float3 position : POS;
    float3 normal   : NOR;
    float3 tangent  : TAN;
    float4 color    : COL;
    float2 texcoord : TEX;
};

struct PixelData
{
    float4 position  : SV_POSITION;
    float2 texcoord  : TEX;
    float4 color     : COL;
    float3x3 tbn     : TBN;
};

Texture2D    _Albedo_tex        : register(t0);
Texture2D    _Normal_tex        : register(t1);

SamplerState _Default_Sampler   : register(s0);

PixelData vert(VertexData vertex)
{
    PixelData output;

    output.position = mul(mul(PF_worldToClip, PO_transform), float4(vertex.position, 1.0f));
    output.texcoord = vertex.texcoord;
    output.color = float4(vertex.color.rgb, 1.0f);

    float3 normal       = normalize(mul(PO_transform, float4(vertex.normal, 0.0)).xyz);
    float3 bitangent    = normalize(mul(PO_transform, float4(cross(vertex.normal, vertex.tangent), 0.0)).xyz);
    float3 tangent      = normalize(mul(PO_transform, float4(vertex.tangent, 0.0)).xyz);

    output.tbn = float3x3(tangent, bitangent, normal);

    return output;
}

float4 frag(PixelData pixel) : SV_TARGET
{
    const float4 normalTex = _Normal_tex.Sample(_Default_Sampler, pixel.texcoord);
    const float3 normal = normalize(mul(pixel.tbn, normalize(normalTex.rgb * 2.0 - 1.0)));

    return float4(normal, 1.0);
    //return _Albedo_tex.Sample(_Default_Sampler, pixel.texcoord);
}