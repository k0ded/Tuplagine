#include "tgpch.h"
#include "Vertex.h"
#include <d3d11.h>
#include <vector>

const std::vector<Tupla::VertexElementDesc> Tupla::Vertex::InputLayoutDefinition =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, false },
	{ "NORMAL", 0, DXGI_FORMAT_R8G8B8A8_SNORM, false },
	{ "TANGENT", 0, DXGI_FORMAT_R8G8B8A8_SNORM, false },
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, false },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, false },
};