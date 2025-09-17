#pragma once
#include "Types.h"
#include "Containers.h"
#include "Vertex.h"

struct FStaticMeshRenderData
{
    TArray<FVertex> Vertices;
    TArray<uint32> Indices;
    
    uint32 NumVertices;
    uint32 NumTriangles;
    
    FStaticMeshRenderData()
        : NumVertices(0)
        , NumTriangles(0)
    {}
    
    FStaticMeshRenderData(const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices)
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