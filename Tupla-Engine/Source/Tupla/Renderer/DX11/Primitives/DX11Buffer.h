#pragma once
#include <d3d11.h>

#include "Tupla/Renderer/Buffer.h"

namespace Tupla
{
	class DX11Renderer;
}

namespace Tupla {

	class DX11Buffer : public Buffer
	{
	public:
		DX11Buffer(DX11Renderer* renderer, u32 size);
		~DX11Buffer() override;

		bool Map() override;
		void Unmap() override;

		void WriteToBuffer(const void* data, u64 size, u64 offset) override;

	private:
		DX11Renderer* m_Renderer;

		D3D11_MAPPED_SUBRESOURCE m_MappedResource;

		u64 m_Size;
	};
}

