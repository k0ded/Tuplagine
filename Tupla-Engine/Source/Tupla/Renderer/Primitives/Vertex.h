#pragma once
#include <vector>
#include "CommonUtilities/Math/Vector/Vector.h"
#include "../RHI/RHIStructs.hpp"

namespace Tupla
{
	struct Vertex
	{
		CU::Vector3f Position = { 0, 0, 0 };			// 12 bytes
		CU::Vector4b Normal = { 0, 0, 0, 0 };		// 4 bytes
		CU::Vector4b Tangent = { 0, 0, 0, 0 };		// 4 bytes
		CU::Vector4b Color = { 0, 0, 0, 1 };		// 4 bytes
		CU::Vector2f UV = {};									// 8 bytes

		Vertex() = default;
		Vertex(const CU::Vector3f& position, const CU::Vector3f& normal = {}, const CU::Vector3f& tangent = {}, const CU::Vector4f& color = {}, const CU::Vector2f& uv = {})
		{
			Position = position;
			Normal = CU::Pack4b(normal.GetNormalized().ToVec4());
			Tangent = CU::Pack4b(tangent.GetNormalized().ToVec4());
			Color = CU::Pack4b(color);
			UV = uv;
		}

		static const std::vector<VertexElementDesc> InputLayoutDefinition;
	};
}
