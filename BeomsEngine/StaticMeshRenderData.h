#pragma once
#include "Types.h"
#include "Containers.h"
#include "Vertex.h"

struct FObjMaterialInfo
{
    FString MaterialName;
    FString DiffuseTexturePath;
    FString NormalTexturePath;
    FString SpecularTexturePath;
    FVector AmbientColorScalar = FVector(0.2f, 0.2f, 0.2f);
    FVector DiffuseColorScalar = FVector(0.8f, 0.8f, 0.8f);
    FVector SpecularColorScalar = FVector(1.0f, 1.0f, 1.0f);
    float ShininessScalar = 32.0f;
    float TransparencyScalar = 1.0f;

    FObjMaterialInfo() = default;

    /**
     * @brief 재질 이름으로 생성하는 생성자
     * @param InMaterialName 생성할 재질의 이름
     */
    FObjMaterialInfo(const FString& InMaterialName)
        : MaterialName(InMaterialName)
    {
    }
};

struct FObjInfo
{
    FString ObjName;							// 객체 이름 (OBJ 'o' 또는 'g' 명령어에서 가져옴)
    TArray<FVector> VertexList;					// OBJ 'v' 명령어의 원시 정점 위치
    TArray<FVector2> UVList;					// OBJ 'vt' 명령어의 원시 UV 좌표
    TArray<FVector> NormalList;					// OBJ 'vn' 명령어의 원시 노멀 벡터
    TArray<uint32> VertexIndexList;				// OBJ 'f' 명령어의 정점 인덱스 (VertexList 참조)
    TArray<uint32> UVIndexList;					// OBJ 'f' 명령어의 UV 인덱스 (UVList 참조)
    TArray<uint32> NormalIndexList;				// OBJ 'f' 명령어의 노멀 인덱스 (NormalList 참조)
    TArray<FObjMaterialInfo> MaterialList;	    // 이 객체에서 사용하는 재질 목록
    TArray<FString> TextureList;				// 이 객체에서 사용하는 텍스처 파일 경로 목록

    FObjInfo() = default;

    FObjInfo(const FString& InObjectName)
        : ObjName(InObjectName)
    {
    }
};

struct FStaticMaterial
{
    FString MaterialSlotName;
    class UMaterialInterface* Material;
    FString ImportedMaterialSlotName;

    FStaticMaterial()
        : Material(nullptr)
    {}

    FStaticMaterial(const FString& InSlotName, class UMaterialInterface* InMaterial = nullptr)
        : MaterialSlotName(InSlotName)
        , Material(InMaterial)
        , ImportedMaterialSlotName(InSlotName)
    {}
};

struct FStaticMeshSection
{
    uint32 MaterialIndex;
    uint32 FirstIndex;
    uint32 NumTriangles;
    uint32 MinVertexIndex;
    uint32 MaxVertexIndex;

    FStaticMeshSection()
        : MaterialIndex(0)
        , FirstIndex(0)
        , NumTriangles(0)
        , MinVertexIndex(0)
        , MaxVertexIndex(0)
    {}

    FStaticMeshSection(uint32 InMaterialIndex, uint32 InFirstIndex, uint32 InNumTriangles, uint32 InMinVertexIndex, uint32 InMaxVertexIndex)
        : MaterialIndex(InMaterialIndex)
        , FirstIndex(InFirstIndex)
        , NumTriangles(InNumTriangles)
        , MinVertexIndex(InMinVertexIndex)
        , MaxVertexIndex(InMaxVertexIndex)
    {}
};

struct FStaticMesh
{
    FString PathFileName;

    TArray<FVertex> Vertices;
    TArray<uint32> Indices;

    TArray<FStaticMaterial> StaticMaterials;
    TArray<FStaticMeshSection> Sections;

    uint32 NumVertices;
    uint32 NumTriangles;
    
    FStaticMesh()
        : NumVertices(0)
        , NumTriangles(0)
    {}
    
    FStaticMesh(const FString& InPathFileName, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices)
        : PathFileName(InPathFileName)
        , Vertices(InVertices)
        , Indices(InIndices)
        , NumVertices(static_cast<uint32>(InVertices.size()))
        , NumTriangles(static_cast<uint32>(InIndices.size() / 3))
    {
        // 기본 머티리얼 슬롯과 섹션 생성
        if (Indices.size() > 0)
        {
            StaticMaterials.push_back(FStaticMaterial("DefaultMaterial"));
            Sections.push_back(FStaticMeshSection(0, 0, NumTriangles, 0, NumVertices > 0 ? NumVertices - 1 : 0));
        }
    }
    
    void UpdateCounts()
    {
        NumVertices = static_cast<uint32>(Vertices.size());
        NumTriangles = static_cast<uint32>(Indices.size() / 3);
    }

    void AddMaterialSlot(const FString& SlotName, class UMaterialInterface* Material = nullptr)
    {
        StaticMaterials.push_back(FStaticMaterial(SlotName, Material));
    }

    void AddSection(uint32 MaterialIndex, uint32 FirstIndex, uint32 NumTriangles, uint32 MinVertexIndex, uint32 MaxVertexIndex)
    {
        Sections.push_back(FStaticMeshSection(MaterialIndex, FirstIndex, NumTriangles, MinVertexIndex, MaxVertexIndex));
    }

    int32 GetMaterialIndex(const FString& MaterialSlotName) const
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
};

