#include "pch.h"
#include "StaticMesh.h"
#include "MaterialInterface.h"
#include "StaticMeshRenderResource.h"

IMPLEMENT_CLASS(UStaticMesh, UObject)

UStaticMesh::UStaticMesh()
	: RenderResource(nullptr)
{
}

UStaticMesh::~UStaticMesh()
{
	if (RenderResource)
	{
		delete RenderResource;
		RenderResource = nullptr;
	}
}

void UStaticMesh::SetRenderData(const FStaticMeshRenderData& InRenderData)
{
	RenderData = InRenderData;
}

void UStaticMesh::SetRenderData(const FString& InFilePath, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices)
{
	RenderData = FStaticMeshRenderData(InFilePath, InVertices, InIndices);
}

void UStaticMesh::InitializeRenderResources(ID3D11Device* Device)
{
	if (!Device || !HasValidRenderData())
	{
		return;
	}

	// RenderResource 생성 (없으면)
	if (!RenderResource)
	{
		RenderResource = new FStaticMeshRenderResource();
	}

	// GPU 리소스 초기화
	RenderResource->InitializeResources(Device, &RenderData);
}

void UStaticMesh::AddMaterialSlot(UMaterialInterface* Material)
{
	Materials.push_back(Material);
}

void UStaticMesh::SetMaterial(int32 MaterialIndex, UMaterialInterface* Material)
{
	if (MaterialIndex >= 0 && MaterialIndex < Materials.size())
	{
		Materials[MaterialIndex] = Material;
	}
}

UMaterialInterface* UStaticMesh::GetMaterial(int32 MaterialIndex) const
{
	if (MaterialIndex >= 0 && MaterialIndex < Materials.size())
	{
		return Materials[MaterialIndex];
	}
	return nullptr;
}

void UStaticMesh::AddSection(uint32 MaterialIndex, uint32 FirstIndex, uint32 NumTriangles, uint32 MinVertexIndex, uint32 MaxVertexIndex)
{
	Sections.push_back(FStaticMeshSection(MaterialIndex, FirstIndex, NumTriangles, MinVertexIndex, MaxVertexIndex));
}

bool UStaticMesh::HasValidRenderData() const
{
	return RenderData.NumVertices > 0 && RenderData.NumTriangles > 0 &&
		   !RenderData.Vertices.empty() && !RenderData.Indices.empty();
}

FVector UStaticMesh::GetBoundingBoxMin() const
{
	return RenderData.GetBoundingBoxMin();
}

FVector UStaticMesh::GetBoundingBoxMax() const
{
	return RenderData.GetBoundingBoxMax();
}

FVector UStaticMesh::GetBoundingBoxCenter() const
{
	return RenderData.GetBoundingBoxCenter();
}

FVector UStaticMesh::GetBoundingBoxExtent() const
{
	return RenderData.GetBoundingBoxExtent();
}

float UStaticMesh::GetBoundingSphereRadius() const
{
	return RenderData.GetBoundingSphereRadius();
}

const FBoxSphereBounds& UStaticMesh::GetBounds() const
{
	return RenderData.Bounds;
}

void UStaticMesh::BuildFromObjData(const FObjInfo& ObjData)
{
	// 1. OBJ 데이터를 FVertex 배열로 변환
	TArray<FVertex> Vertices;
	TArray<uint32> Indices;

	// OBJ 파일의 면(face) 데이터를 기반으로 정점 생성
	for (size_t i = 0; i < ObjData.VertexIndexList.size(); ++i)
	{
		FVertex NewVertex;

		// 위치 데이터
		if (ObjData.VertexIndexList[i] < ObjData.VertexList.size())
		{
			NewVertex.Position = ObjData.VertexList[ObjData.VertexIndexList[i]];
		}

		// UV 데이터
		if (i < ObjData.UVIndexList.size() && ObjData.UVIndexList[i] < ObjData.UVList.size())
		{
			NewVertex.UV = ObjData.UVList[ObjData.UVIndexList[i]];
		}

		// 노멀 데이터
		if (i < ObjData.NormalIndexList.size() && ObjData.NormalIndexList[i] < ObjData.NormalList.size())
		{
			NewVertex.Normal = ObjData.NormalList[ObjData.NormalIndexList[i]];
		}

		Vertices.push_back(NewVertex);
		Indices.push_back(static_cast<uint32>(i));
	}

	// 2. 렌더링 데이터 설정 (파일 경로 포함)
	RenderData = FStaticMeshRenderData(ObjData.ObjName, Vertices, Indices);

	// 3. 머티리얼 및 섹션 생성
	BuildDefaultMaterialsAndSections();
}

void UStaticMesh::BuildDefaultMaterialsAndSections()
{
	// 기존 머티리얼과 섹션 초기화
	Materials.clear();
	Sections.clear();

	if (HasValidRenderData())
	{
		// 기본 머티리얼 슬롯 생성 (nullptr로 시작, 나중에 설정)
		AddMaterialSlot(nullptr);

		// 전체 메시에 대한 단일 섹션 생성
		AddSection(
			0,                                                           // MaterialIndex
			0,                                                           // FirstIndex
			RenderData.NumTriangles,                                     // NumTriangles
			0,                                                           // MinVertexIndex
			RenderData.NumVertices > 0 ? RenderData.NumVertices - 1 : 0  // MaxVertexIndex
		);
	}
}
