#pragma once
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
    FName MaterialSlotName;
    class UMaterialInterface* Material;
    FName ImportedMaterialSlotName;

    FStaticMaterial()
        : Material(nullptr)
    {}

    FStaticMaterial(const FName& InSlotName, class UMaterialInterface* InMaterial = nullptr)
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

struct FStaticMeshRenderData
{
    FString SourceFilePath;

    TArray<FVertex> Vertices;
    TArray<uint32> Indices;

    uint32 NumVertices;
    uint32 NumTriangles;

    //struct FBoxSphereBounds Bounds;

    FStaticMeshRenderData()
        : NumVertices(0)
        , NumTriangles(0)
    {}

    FStaticMeshRenderData(const FString& InPathFileName, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices)
        : SourceFilePath(InPathFileName)
        , Vertices(InVertices)
        , Indices(InIndices)
        , NumVertices(static_cast<uint32>(InVertices.size()))
        , NumTriangles(static_cast<uint32>(InIndices.size() / 3))
    {
    }

    void UpdateCounts()
    {
        NumVertices = static_cast<uint32>(Vertices.size());
        NumTriangles = static_cast<uint32>(Indices.size() / 3);
    }
};

