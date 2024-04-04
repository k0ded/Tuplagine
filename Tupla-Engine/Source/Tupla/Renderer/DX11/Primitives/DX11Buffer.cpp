#include "tgpch.h"
#include "DX11Buffer.h"

#include "Tupla/Renderer/DX11/DX11Renderer.h"

Tupla::DX11Buffer::DX11Buffer(DX11Renderer* renderer, u32 size): m_Renderer(renderer), m_Size(size)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = size + 0xf & 0xfffffff0; // ensure buffer size is multiple of 16 bytes;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

#ifdef DEBUG
#if BUFFER_PERFORMANCE_WARN
	constexpr u32 warningThreshold = 1;

	if(size % 16 < warningThreshold && size % 16 != 0)
	{
		LOG_WARN("PERFORMANCE: Constant buffer is close to being one multiple of 16 bytes less! Consider compressing data where possible.");
	}
#endif
#endif

	const auto result = renderer->GetDevice()->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_GPUMemory);
	ASSERT(SUCCEEDED(result), "Failed to create constant buffer")
}

Tupla::DX11Buffer::~DX11Buffer()
{
	((ID3D11Buffer*)m_GPUMemory)->Release();
}

bool Tupla::DX11Buffer::Map()
{
	const auto result = m_Renderer->GetDeviceContext()->Map((ID3D11Buffer*)m_GPUMemory, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_MappedResource);
	m_MappedMemory = m_MappedResource.pData;

	return SUCCEEDED(result);
}

void Tupla::DX11Buffer::Unmap()
{
	m_Renderer->GetDeviceContext()->Unmap((ID3D11Buffer*)m_GPUMemory, 0);
	m_MappedMemory = nullptr;
}

void Tupla::DX11Buffer::WriteToBuffer(const void* data, u64 size, u64 offset)
{
	ASSERT(size + offset <= m_Size, "Cannot write past buffers end");

	if (!Map()) 
	{
		LOG_ERROR("Failed to map memory when writing to buffer");
		return;
	}

	memcpy((char*)m_MappedMemory + offset, data, size);
	Unmap();
}
