#pragma once
#include "IComponent.h"
#include <Tupla/Renderer/Camera.h>

class CameraComponent : public IComponent
{
public:
	CLASS_COMPONENT_TYPE(CameraComponent);

	void Update() override;
	void Render();

private:
	Tupla::Camera camera;
};

