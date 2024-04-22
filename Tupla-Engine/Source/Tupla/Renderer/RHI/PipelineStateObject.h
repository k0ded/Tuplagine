#pragma once
#include "wrl.h"

class Shader;

struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11InputLayout;

namespace Tupla
{
	struct PSOTextureShaderResource
	{
		u32 stage;
		u32 slot;
		bool writable;

		std::shared_ptr<Texture> texture;
	};

	struct PSOStructuredShaderResource
	{
		u32 stage;
		u32 slot;
		bool writable;

		std::shared_ptr<ComputeBuffer> buffer;
	};

	struct PipelineStateObject
	{
		std::weak_ptr<Shader> VertexShader;
		std::weak_ptr<Shader> PixelShader;
		std::weak_ptr<Shader> GeometryShader;

		std::vector<std::shared_ptr<Texture>> RenderTargets;
		std::vector<PSOTextureShaderResource> PipelineTextureShaderResources;
		std::vector<PSOStructuredShaderResource> PipelineStructuredShaderResources;
		std::shared_ptr<Texture> DepthStencil;

		u32 Topology;
		bool ClearRenderTargets;
		bool ClearDepthStencil;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
		Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	};

	struct ComputePipelineStateObject
	{
		std::weak_ptr<Shader> ComputeShader;

		std::vector<PSOTextureShaderResource> PipelineTextureShaderResources;
		std::vector<PSOStructuredShaderResource> PipelineStructuredShaderResources;
	};
}