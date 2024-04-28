cbuffer FrameBuffer : register(b0) 
{
	float4x4 FB_InvView;
	float4x4 FB_Projection;
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 OB_Transform;
}

struct VertexInput
{
	float3 position			: POSITION;
	unorm float4 normal		: NORMAL;
	unorm float4 tangent	: TANGENT;
	unorm float4 color		: COLOR;
	float2 uv				: TEXCOORD;
};

struct VertexToPixel 
{
	float4 position			: SV_POSITION;
	unorm float4 normal		: NORMAL;
	unorm float4 tangent	: TANGENT;
	unorm float4 color		: COLOR;
	float2 uv				: TEXCOORD;
};