#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Containers.h"

using Microsoft::WRL::ComPtr;

// Shader Stage Enum for Constant Buffer Binding
enum class EShaderStage
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute
};

// Template-based Generic DirectX 11 Buffer Wrapper
template<typename T>
class TBuffer
{
private:
	ComPtr<ID3D11Buffer> Buffer;
	UINT Stride;
	UINT Count;

public:
	TBuffer()
		: Stride(sizeof(T))
		, Count(0)
	{
	}

	~TBuffer()
	{
		Release();
	}

	// Non-copyable
	TBuffer(const TBuffer&) = delete;
	TBuffer& operator=(const TBuffer&) = delete;

	// Movable
	TBuffer(TBuffer&& Other) noexcept
		: Buffer(std::move(Other.Buffer))
		, Stride(Other.Stride)
		, Count(Other.Count)
	{
		Other.Count = 0;
	}

	TBuffer& operator=(TBuffer&& Other) noexcept
	{
		if (this != &Other)
		{
			Release();
			Buffer = std::move(Other.Buffer);
			Stride = Other.Stride;
			Count = Other.Count;
			Other.Count = 0;
		}
		return *this;
	}

	// Initialize buffer with data
	bool Initialize(
		ID3D11Device* Device,
		const TArray<T>& Data,
		UINT BindFlags,
		D3D11_USAGE Usage = D3D11_USAGE_DEFAULT
	);

	// Update buffer data (for Dynamic buffers)
	void Update(ID3D11DeviceContext* Context, const T& Data);

	// Binding helpers
	void BindAsVertexBuffer(ID3D11DeviceContext* Context, UINT Slot = 0);
	void BindAsIndexBuffer(ID3D11DeviceContext* Context);
	void BindAsConstantBuffer(ID3D11DeviceContext* Context, UINT Slot, EShaderStage Stage);

	// Release buffer
	void Release()
	{
		Buffer.Reset();
		Count = 0;
	}

	// Getters
	ID3D11Buffer* Get() const { return Buffer.Get(); }
	ID3D11Buffer** GetAddressOf() { return Buffer.GetAddressOf(); }
	UINT GetStride() const { return Stride; }
	UINT GetCount() const { return Count; }
	UINT GetByteWidth() const { return Stride * Count; }
	bool IsValid() const { return Buffer.Get() != nullptr; }
};

// Template Method Implementations
template<typename T>
bool TBuffer<T>::Initialize(
	ID3D11Device* Device,
	const TArray<T>& Data,
	UINT BindFlags,
	D3D11_USAGE Usage)
{
	if (!Device || Data.empty())
	{
		return false;
	}

	// Release existing buffer
	Release();

	// Auto-calculate buffer properties
	Stride = sizeof(T);
	Count = static_cast<UINT>(Data.size());
	UINT ByteWidth = Stride * Count;

	// Setup buffer description
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = ByteWidth;
	Desc.Usage = Usage;
	Desc.BindFlags = BindFlags;
	Desc.CPUAccessFlags = (Usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	// Setup initial data
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Data.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create buffer
	HRESULT hr = Device->CreateBuffer(&Desc, &InitData, Buffer.GetAddressOf());

	if (FAILED(hr))
	{
		Count = 0;
		return false;
	}

	return true;
}

template<typename T>
void TBuffer<T>::Update(ID3D11DeviceContext* Context, const T& Data)
{
	if (!Context || !Buffer)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// Map with DISCARD for optimal performance
	HRESULT hr = Context->Map(
		Buffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource
	);

	if (SUCCEEDED(hr))
	{
		// Copy data to GPU
		memcpy(MappedResource.pData, &Data, sizeof(T));

		// Unmap
		Context->Unmap(Buffer.Get(), 0);
	}
}

template<typename T>
void TBuffer<T>::BindAsVertexBuffer(ID3D11DeviceContext* Context, UINT Slot)
{
	if (!Context || !Buffer)
	{
		return;
	}

	UINT Offset = 0;
	Context->IASetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &Stride, &Offset);
}

template<typename T>
void TBuffer<T>::BindAsIndexBuffer(ID3D11DeviceContext* Context)
{
	if (!Context || !Buffer)
	{
		return;
	}

	// Auto-determine index format based on sizeof(T)
	DXGI_FORMAT Format = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	Context->IASetIndexBuffer(Buffer.Get(), Format, 0);
}

template<typename T>
void TBuffer<T>::BindAsConstantBuffer(ID3D11DeviceContext* Context, UINT Slot, EShaderStage Stage)
{
	if (!Context || !Buffer)
	{
		return;
	}

	ID3D11Buffer* Buffers[] = { Buffer.Get() };

	switch (Stage)
	{
	case EShaderStage::Vertex:
		Context->VSSetConstantBuffers(Slot, 1, Buffers);
		break;
	case EShaderStage::Pixel:
		Context->PSSetConstantBuffers(Slot, 1, Buffers);
		break;
	case EShaderStage::Geometry:
		Context->GSSetConstantBuffers(Slot, 1, Buffers);
		break;
	case EShaderStage::Hull:
		Context->HSSetConstantBuffers(Slot, 1, Buffers);
		break;
	case EShaderStage::Domain:
		Context->DSSetConstantBuffers(Slot, 1, Buffers);
		break;
	case EShaderStage::Compute:
		Context->CSSetConstantBuffers(Slot, 1, Buffers);
		break;
	}
}
