#pragma once
#include "Types.h"
#include "Containers.h"
#include "Vertex.h"
#include "StaticMeshRenderData.h"

// 기본 메쉬 생성기
class UKismetProceduralMeshLibrary
{
public:
    static FStaticMesh CreateCubeMesh(FVector BoxRadius = FVector(50.0f, 50.0f, 50.0f));

    static FStaticMesh CreateSphereMesh(float SphereRadius = 50.0f, int32 SphereSegments = 32, int32 SphereRings = 16);

    static FStaticMesh CreateCylinderMesh(float CylinderRadius = 50.0f, float CylinderHeight = 100.0f, int32 CylinderSegments = 32);

    static FStaticMesh CreateConeMesh(float ConeRadius = 50.0f, float ConeHeight = 100.0f, int32 ConeSegments = 32);

    static FStaticMesh CreatePlaneMesh(FVector PlaneSize = FVector(100.0f, 100.0f, 0.0f), int32 WidthSegments = 1, int32 HeightSegments = 1);

private:
    static constexpr float PI = 3.14159265359f;
    static constexpr int32 DefaultSegments = 32;
};