#include "tgpch.h"
#include "Camera.h"

#include "RHI/RHI.h"

void Tupla::Camera::Update() const
{
	if (!Buffer) [[unlikely]]
		return;

	Data = localCopy;
	RHI::UpdateConstantBufferData(*Buffer);
}

void Tupla::Camera::SetPerspectiveProjection(float aAspectRatio, float aHorizontalFov, float aFar, float aNear)
{
	localCopy.Projection = CU::Matrix4x4<float>::CreatePerspectiveProjection(aAspectRatio, aHorizontalFov, aFar, aNear);
}

void Tupla::Camera::SetOrthographicProjection(float aWidth, float aHeight, float aFar, float aNear)
{
	localCopy.Projection = CU::Matrix4x4<float>::CreateOrthographicProjection(aWidth, aHeight, aFar, aNear);
}

void Tupla::Camera::SetTransform(const CU::Matrix4x4<float>& aTransform)
{
	localCopy.InvView = aTransform.GetFastInverse();
}

void Tupla::Camera::Bind()
{
	if(!Buffer) [[unlikely]]
	{
		Buffer = CreateRef<ConstantBuffer>(
			ConstantBuffer::CreateBuffer(
				"FrameBuffer",
				PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,
				0,
				&Data
			)
		);
	}

	Buffer->Bind();
}
