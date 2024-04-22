#pragma once
#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"

namespace Tupla
{
	struct FrameBuffer
	{
		CU::Matrix4x4<float> InvView;
		CU::Matrix4x4<float> Projection;
	};
}
