#pragma once

// Meshes - 모든 메쉬 관련 헤더를 포함하는 통합 헤더
#include "StaticMeshVertex.h"
#include "StaticMeshRenderData.h"
#include "KismetProceduralMeshLibrary.h"

namespace Meshes
{
    using MeshData = FStaticMeshRenderData;
    using Vertex = FStaticMeshVertex;
    using MeshLibrary = UKismetProceduralMeshLibrary;
}