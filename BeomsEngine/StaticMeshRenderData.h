#pragma once
#include "Types.h"
#include "Containers.h"
#include "StaticMeshVertex.h"

struct FStaticMeshRenderData
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
    
    // DirectX11 특화 데이터
    uint32 NumVertices;
    uint32 NumTriangles;
    
    FStaticMeshRenderData()
        : NumVertices(0)
        , NumTriangles(0)
    {}
    
    FStaticMeshRenderData(const TArray<FStaticMeshVertex>& InVertices, const TArray<uint32>& InIndices)
        : Vertices(InVertices)
        , Indices(InIndices)
        , NumVertices(static_cast<uint32>(InVertices.size()))
        , NumTriangles(static_cast<uint32>(InIndices.size() / 3))
    {}
    
    void UpdateCounts()
    {
        NumVertices = static_cast<uint32>(Vertices.size());
        NumTriangles = static_cast<uint32>(Indices.size() / 3);
    }
};