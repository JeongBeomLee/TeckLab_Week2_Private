#include "pch.h"
#include "StaticMeshRenderResource.h"
#include "StaticMeshRenderData.h"
#include "BufferManager.h"

FStaticMeshRenderResource::FStaticMeshRenderResource()
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, bInitialized(false)
{
}

FStaticMeshRenderResource::~FStaticMeshRenderResource()
{
	ReleaseResources();
}

bool FStaticMeshRenderResource::InitializeResources(ID3D11Device* Device, const FStaticMeshRenderData* RenderData)
{
	if (!Device || !RenderData)
	{
		return false;
	}

	if (RenderData->Vertices.empty() || RenderData->Indices.empty())
	{
		return false;
	}

	// 기존 리소스 해제
	ReleaseResources();

	// FBufferManager를 통해 Vertex Buffer 가져오기 (캐싱됨)
	VertexBuffer = FBufferManager::Get().GetOrCreateVertexBuffer(RenderData->Vertices);
	if (!VertexBuffer)
	{
		return false;
	}

	// FBufferManager를 통해 Index Buffer 가져오기 (캐싱됨)
	IndexBuffer = FBufferManager::Get().GetOrCreateIndexBuffer(RenderData->Indices);
	if (!IndexBuffer)
	{
		VertexBuffer = nullptr;
		return false;
	}

	bInitialized = true;
	return true;
}

void FStaticMeshRenderResource::ReleaseResources()
{
	if (bInitialized)
	{
		// 참조만 해제 (실제 버퍼는 FBufferManager가 소유)
		VertexBuffer = nullptr;
		IndexBuffer = nullptr;
		bInitialized = false;
	}
}

void FStaticMeshRenderResource::BindBuffers(ID3D11DeviceContext* Context)
{
	if (!Context || !bInitialized || !VertexBuffer || !IndexBuffer)
	{
		return;
	}

	// Vertex Buffer 바인딩
	VertexBuffer->BindAsVertexBuffer(Context, 0);

	// Index Buffer 바인딩
	IndexBuffer->BindAsIndexBuffer(Context);
}

void FStaticMeshRenderResource::Draw(ID3D11DeviceContext* Context)
{
	if (!Context || !bInitialized || !IndexBuffer)
	{
		return;
	}

	// DrawIndexed 호출
	Context->DrawIndexed(IndexBuffer->GetCount(), 0, 0);
}
