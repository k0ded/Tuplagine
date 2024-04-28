#include "ShaderCommon.hlsli"

VertexToPixel vert(in VertexInput IN) 
{
	VertexToPixel output;
    float4 position = mul(FB_InvView, float4(IN.position, 1));
    position = mul(FB_Projection, position);
    
    output.position = position;
    output.normal = IN.normal;
    output.tangent = IN.tangent;
    output.color = IN.color;
    output.uv = IN.uv;

	return output;
}

float4 frag(in VertexToPixel IN) : SV_Target
{
    return IN.color;
}