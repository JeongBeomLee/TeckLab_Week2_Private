#include "pch.h"
#include "BufferManager.h"
#include <functional>

FBufferManager* FBufferManager::Instance = nullptr;

FBufferManager::FBufferManager()
	: Device(nullptr)
{
}

FBufferManager::~FBufferManager()
{
	ClearCache();
}

FBufferManager& FBufferManager::Get()
{
	if (!Instance)
	{
		Instance = new FBufferManager();
	}
	return *Instance;
}

void FBufferManager::Initialize(ID3D11Device* InDevice)
{
	if (!Instance)
	{
		Instance = new FBufferManager();
	}
	Instance->Device = InDevice;
}

void FBufferManager::Shutdown()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}

TBuffer<FVertex>* FBufferManager::GetOrCreateVertexBuffer(const TArray<FVertex>& Vertices)
{
	if (Vertices.empty())
	{
		return nullptr;
	}

	// 해시 계산
	size_t Hash = HashVertexData(Vertices);

	// 캐시 확인
	auto it = VertexBufferCache.find(Hash);
	if (it != VertexBufferCache.end())
	{
		return it->second;
	}

	// 새로 생성
	TBuffer<FVertex>* NewBuffer = new TBuffer<FVertex>();
	if (!NewBuffer->Initialize(Device, Vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT))
	{
		delete NewBuffer;
		return nullptr;
	}

	// 캐싱
	VertexBufferCache[Hash] = NewBuffer;

	return NewBuffer;
}

TBuffer<uint32>* FBufferManager::GetOrCreateIndexBuffer(const TArray<uint32>& Indices)
{
	if (Indices.empty())
	{
		return nullptr;
	}

	// 해시 계산
	size_t Hash = HashIndexData(Indices);

	// 캐시 확인
	auto it = IndexBufferCache.find(Hash);
	if (it != IndexBufferCache.end())
	{
		return it->second;
	}

	// 새로 생성
	TBuffer<uint32>* NewBuffer = new TBuffer<uint32>();
	if (!NewBuffer->Initialize(Device, Indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT))
	{
		delete NewBuffer;
		return nullptr;
	}

	// 캐싱
	IndexBufferCache[Hash] = NewBuffer;

	return NewBuffer;
}

void FBufferManager::ClearCache()
{
	// Vertex Buffer 해제
	for (auto& Pair : VertexBufferCache)
	{
		if (Pair.second)
		{
			delete Pair.second;
		}
	}
	VertexBufferCache.clear();

	// Index Buffer 해제
	for (auto& Pair : IndexBufferCache)
	{
		if (Pair.second)
		{
			delete Pair.second;
		}
	}
	IndexBufferCache.clear();

	// Constant Buffer 해제
	for (auto& Pair : ConstantBufferCache)
	{
		if (Pair.second)
		{
			// void* 이므로 타입별 삭제 불가, 메모리 누수 방지를 위해 경고
			// 실제로는 타입 정보 저장 필요하지만 일단 단순화
			delete Pair.second;
		}
	}
	ConstantBufferCache.clear();
}

size_t FBufferManager::HashVertexData(const TArray<FVertex>& Vertices)
{
	// 간단한 해시 함수 (데이터 바이트 기반)
	const char* Data = reinterpret_cast<const char*>(Vertices.data());
	size_t Size = Vertices.size() * sizeof(FVertex);

	std::hash<std::string> Hasher;
	return Hasher(std::string(Data, Size));
}

size_t FBufferManager::HashIndexData(const TArray<uint32>& Indices)
{
	// 간단한 해시 함수 (데이터 바이트 기반)
	const char* Data = reinterpret_cast<const char*>(Indices.data());
	size_t Size = Indices.size() * sizeof(uint32);

	std::hash<std::string> Hasher;
	return Hasher(std::string(Data, Size));
}
