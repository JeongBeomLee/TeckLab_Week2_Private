#pragma once
#include <d3d11.h>
#include "TBuffer.h"
#include "Vertex.h"

struct FStaticMeshRenderData;

// GPU 렌더링 리소스 클래스 (CPU 데이터인 FStaticMeshRenderData와 분리)
// FBufferManager를 통해 버퍼를 캐싱하여 중복 제거
class FStaticMeshRenderResource
{
private:
	TBuffer<FVertex>* VertexBuffer;   // FBufferManager에서 관리 (소유하지 않음)
	TBuffer<uint32>* IndexBuffer;     // FBufferManager에서 관리 (소유하지 않음)
	bool bInitialized;

public:
	FStaticMeshRenderResource();
	~FStaticMeshRenderResource();

	// CPU 렌더 데이터로부터 GPU 버퍼 초기화 (FBufferManager 사용)
	bool InitializeResources(ID3D11Device* Device, const FStaticMeshRenderData* RenderData);

	// 리소스 해제 (참조만 해제, 실제 버퍼는 FBufferManager가 관리)
	void ReleaseResources();

	// Input Assembler 스테이지에 버텍스/인덱스 버퍼 바인딩
	void BindBuffers(ID3D11DeviceContext* Context);

	// Draw 호출
	void Draw(ID3D11DeviceContext* Context);

	// Getters
	bool IsInitialized() const { return bInitialized; }
	TBuffer<FVertex>* GetVertexBuffer() const { return VertexBuffer; }
	TBuffer<uint32>* GetIndexBuffer() const { return IndexBuffer; }
};
