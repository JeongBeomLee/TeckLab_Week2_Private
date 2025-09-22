#include "pch.h"
#include "StaticMesh.h"
#include "MaterialInterface.h"
#include <algorithm>

IMPLEMENT_CLASS(UStaticMesh, UObject);

UStaticMesh::UStaticMesh()
{
}

UStaticMesh::~UStaticMesh()
{
}

void UStaticMesh::SetRenderData(const FStaticMeshRenderData& InRenderData)
{
    RenderData = InRenderData;
}

void UStaticMesh::SetRenderData(const FString& InFilePath, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices)
{
    RenderData = FStaticMeshRenderData(InFilePath, InVertices, InIndices);
}

void UStaticMesh::AddMaterialSlot(const FName& SlotName, UMaterialInterface* Material)
{
    StaticMaterials.push_back(FStaticMaterial(SlotName, Material));
}

void UStaticMesh::SetMaterial(int32 MaterialIndex, UMaterialInterface* Material)
{
    if (MaterialIndex >= 0 && MaterialIndex < StaticMaterials.size())
    {
        StaticMaterials[MaterialIndex].Material = Material;
    }
}

void UStaticMesh::SetMaterialByName(const FName& SlotName, UMaterialInterface* Material)
{
    int32 Index = GetMaterialIndex(SlotName);
    if (Index != -1)
    {
        SetMaterial(Index, Material);
    }
}

UMaterialInterface* UStaticMesh::GetMaterial(int32 MaterialIndex) const
{
    if (MaterialIndex >= 0 && MaterialIndex < StaticMaterials.size())
    {
        return StaticMaterials[MaterialIndex].Material;
    }
    return nullptr;
}

UMaterialInterface* UStaticMesh::GetMaterialByName(const FName& SlotName) const
{
    int32 Index = GetMaterialIndex(SlotName);
    if (Index != -1)
    {
        return GetMaterial(Index);
    }
    return nullptr;
}

int32 UStaticMesh::GetMaterialIndex(const FName& MaterialSlotName) const
{
    for (size_t i = 0; i < StaticMaterials.size(); ++i)
    {
        if (StaticMaterials[i].MaterialSlotName == MaterialSlotName)
        {
            return static_cast<int32>(i);
        }
    }
    return -1;
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
    StaticMaterials.clear();
    Sections.clear();

    if (HasValidRenderData())
    {
        // 기본 머티리얼 슬롯 생성
        AddMaterialSlot(FName("DefaultMaterial"));

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

// 편의 함수들 구현
//namespace StaticMeshUtils
//{
//    UStaticMesh* CreateStaticMeshFromObj(const FObjInfo& ObjData, const FString& MeshName)
//    {
//        FName ActualName = MeshName.empty() ? FName(ObjData.ObjName) : FName(MeshName);
//        UStaticMesh* NewMesh = NewObject<UStaticMesh>(nullptr, ActualName);
//
//        if (NewMesh)
//        {
//            NewMesh->BuildFromObjData(ObjData);
//        }
//
//        return NewMesh;
//    }
//
//    UStaticMesh* CreateCubeMesh()
//    {
//        UStaticMesh* CubeMesh = NewObject<UStaticMesh>(nullptr, FName("CubeMesh"));
//
//        if (CubeMesh)
//        {
//            // 간단한 큐브 정점 데이터 생성 (예시)
//            TArray<FVertex> Vertices = {
//                // 앞면
//                { FVector(-1, -1,  1), FVector(0, 0, 1), FVector2(0, 0) },
//                { FVector( 1, -1,  1), FVector(0, 0, 1), FVector2(1, 0) },
//                { FVector( 1,  1,  1), FVector(0, 0, 1), FVector2(1, 1) },
//                { FVector(-1,  1,  1), FVector(0, 0, 1), FVector2(0, 1) }
//                // ... 다른 면들도 추가 가능
//            };
//
//            TArray<uint32> Indices = {
//                0, 1, 2, 2, 3, 0  // 앞면 삼각형 2개
//                // ... 다른 면들의 인덱스도 추가 가능
//            };
//
//            CubeMesh->SetVertices(Vertices);
//            CubeMesh->SetIndices(Indices);
//            CubeMesh->BuildDefaultMaterialsAndSections();
//        }
//
//        return CubeMesh;
//    }
//
//    UStaticMesh* CreateSphereMesh(int32 Segments)
//    {
//        // TODO: 구 메시 생성 구현
//        return nullptr;
//    }
//
//    UStaticMesh* CreatePlaneMesh(float Width, float Height)
//    {
//        UStaticMesh* PlaneMesh = NewObject<UStaticMesh>(nullptr, FName("PlaneMesh"));
//
//        if (PlaneMesh)
//        {
//            float HalfWidth = Width * 0.5f;
//            float HalfHeight = Height * 0.5f;
//
//            TArray<FVertex> Vertices = {
//                { FVector(-HalfWidth, -HalfHeight, 0), FVector(0, 0, 1), FVector2(0, 0) },
//                { FVector( HalfWidth, -HalfHeight, 0), FVector(0, 0, 1), FVector2(1, 0) },
//                { FVector( HalfWidth,  HalfHeight, 0), FVector(0, 0, 1), FVector2(1, 1) },
//                { FVector(-HalfWidth,  HalfHeight, 0), FVector(0, 0, 1), FVector2(0, 1) }
//            };
//
//            TArray<uint32> Indices = {
//                0, 1, 2, 2, 3, 0
//            };
//
//            PlaneMesh->SetVertices(Vertices);
//            PlaneMesh->SetIndices(Indices);
//            PlaneMesh->BuildDefaultMaterialsAndSections();
//        }
//
//        return PlaneMesh;
//    }
//}