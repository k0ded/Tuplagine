#include "tgpch.h"
#include "DX11RenderingAssets.h"

#include "DX11Renderer.h"
#include "Primitives/DX11Buffer.h"
#include "Primitives/DX11Material.h"
#include "Primitives/DX11Mesh.h"
#include "Primitives/DX11Shader.h"
#include "Tupla/Core/Application.h"

Tupla::Scope<Tupla::Material> Tupla::DX11RenderingAssets::CreateMaterial()
{
	auto renderer = (DX11Renderer*)Application::Get().GetRenderer();
	return CreateScope<Tupla::DX11Material>(renderer);
}

Tupla::Scope<Tupla::Shader> Tupla::DX11RenderingAssets::CreateShader()
{
	auto renderer = (DX11Renderer*)Application::Get().GetRenderer();
	return CreateScope<Tupla::DX11Shader>(renderer);;
}

Tupla::Scope<Tupla::Texture> Tupla::DX11RenderingAssets::CreateTexture()
{
	auto renderer = (DX11Renderer*)Application::Get().GetRenderer();
	return CreateScope<Tupla::DX11Texture>(renderer);
}

Tupla::Scope<Tupla::Mesh> Tupla::DX11RenderingAssets::CreateMesh()
{
	auto renderer = (DX11Renderer*)Application::Get().GetRenderer();
	return CreateScope<Tupla::DX11Mesh>(renderer);
}

Tupla::Scope<Tupla::Buffer> Tupla::DX11RenderingAssets::CreateBuffer(u32 byteSize)
{
	auto renderer = (DX11Renderer*)Application::Get().GetRenderer();
	return CreateScope<Tupla::DX11Buffer>(renderer, byteSize);
}
