﻿#include "EditorLayer.h"
#include <imgui.h>

static float someFloat = 0.f;

void Tupla::EditorLayer::OnGUI()
{
	ImGui::Begin("Something");
	ImGui::SliderFloat("slidername", &someFloat, 0, 1);
	ImGui::End();
}
