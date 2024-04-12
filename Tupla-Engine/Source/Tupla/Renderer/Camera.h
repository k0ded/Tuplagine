#pragma once
#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"

namespace Tupla
{
	class Camera
	{
	public:
		void SetDisplaySize(CU::Vector2ui aResolution);
		void SetFOV(float aHorizontalFOV);
		void SetFarPlane(float aFar);
		void SetNearPlane(float aNear);

		void SetPosition(CU::Vector3f aPosition);
		void SetRotation(CU::Vector3f aRotation);

		void PrepareFrame();

	private:
		CU::Matrix4x4<float> m_Perspective{};
		CU::Vector3f m_Position;
		CU::Vector3f m_Rotation;

		float m_Aspect = 0;
		float m_FOV = 0;
		float m_Far = 0;
		float m_Near = 0;
		bool m_PerspectiveDirty = true;
	};
}
