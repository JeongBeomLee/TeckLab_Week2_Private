#pragma once
#include <d3d11.h>
#include "TBuffer.h"
#include "Vertex.h"
#include "Containers.h"
#include "String.h"

// 버퍼 캐싱을 담당하는 싱글톤 클래스
class FBufferManager
{
private:
	static FBufferManager* Instance;

	ID3D11Device* Device;

	// 버퍼 캐싱 (해시 기반 중복 제거)
	TMap<size_t, TBuffer<FVertex>*> VertexBufferCache;
	TMap<size_t, TBuffer<uint32>*> IndexBufferCache;

	// Constant Buffer 캐싱 (이름 기반, void*는 TBuffer<T>*)
	TMap<FString, void*> ConstantBufferCache;

	// 생성자 private (싱글톤)
	FBufferManager();
	~FBufferManager();

	// 복사 방지
	FBufferManager(const FBufferManager&) = delete;
	FBufferManager& operator=(const FBufferManager&) = delete;

public:
	// 싱글톤 접근
	static FBufferManager& Get();
	static void Initialize(ID3D11Device* InDevice);
	static void Shutdown();

	// Vertex/Index Buffer 캐싱 (해시 기반 중복 제거)
	TBuffer<FVertex>* GetOrCreateVertexBuffer(const TArray<FVertex>& Vertices);
	TBuffer<uint32>* GetOrCreateIndexBuffer(const TArray<uint32>& Indices);

	// Constant Buffer (이름 기반)
	template<typename T>
	TBuffer<T>* GetOrCreateConstantBuffer(const FString& BufferName);

	// Device 접근
	ID3D11Device* GetDevice() const { return Device; }

	// 캐시 정리
	void ClearCache();

private:
	// 해시 함수
	size_t HashVertexData(const TArray<FVertex>& Vertices);
	size_t HashIndexData(const TArray<uint32>& Indices);
};

// 템플릿 메서드 구현
template<typename T>
TBuffer<T>* FBufferManager::GetOrCreateConstantBuffer(const FString& BufferName)
{
	// 캐시 확인
	auto it = ConstantBufferCache.find(BufferName);
	if (it != ConstantBufferCache.end())
	{
		return static_cast<TBuffer<T>*>(it->second);
	}

	// 새로 생성 (빈 데이터로 초기화, Dynamic Usage)
	TBuffer<T>* NewBuffer = new TBuffer<T>();
	TArray<T> InitData = { T() };  // 기본 생성자로 1개 요소 초기화

	if (!NewBuffer->Initialize(Device, InitData, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC))
	{
		delete NewBuffer;
		return nullptr;
	}

	// 캐싱
	ConstantBufferCache[BufferName] = NewBuffer;

	return NewBuffer;
}
