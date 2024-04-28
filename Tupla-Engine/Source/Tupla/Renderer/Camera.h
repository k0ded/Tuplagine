#pragma once
#include "Buffers/ConstantBuffer.h"
#include "RHI/FrameBuffer.h"
#include "RHI/RHIStructs.hpp"

namespace Tupla
{
	class Camera
	{
	public:
		void Update() const;

		void SetPerspectiveProjection(float aAspectRatio, float aHorizontalFov, float aFar, float aNear);
		void SetOrthographicProjection(float aWidth, float aHeight, float aFar, float aNear);

		void SetTransform(const CU::Matrix4x4<float>& aTransform);

		static void Bind();
	private:
		FrameBuffer localCopy{};

		static inline FrameBuffer Data {};
		static inline Ref<ConstantBuffer> Buffer;
	};
}
