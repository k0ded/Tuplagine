#pragma once
#include <CommonUtilities/Math/Matrices/Matrix4x4.hpp>

namespace Tupla
{
	struct TransformComponent
	{
		CU::Vector3f WorldPosition;
		CU::Vector3f WorldRotationRad;
		CU::Vector3f WorldScale;

		CU::Vector3f LocalPosition;
		CU::Vector3f LocalRotationRad;
		CU::Vector3f LocalScale;
		bool isDirty;
	};
}
