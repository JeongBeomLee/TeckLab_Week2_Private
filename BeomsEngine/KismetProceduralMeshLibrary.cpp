#include "pch.h"
#include "KismetProceduralMeshLibrary.h"

FStaticMeshRenderData UKismetProceduralMeshLibrary::CreateBoxMesh(FVector BoxRadius)
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
    
    // Unreal Engine 단위계 (cm) 사용
    float X = BoxRadius.X;
    float Y = BoxRadius.Y;
    float Z = BoxRadius.Z;

    // 정점 정의 (Z-Up, 왼손 좌표계)
    TArray<FVector> Positions = {
        // Front face (+Y)
        FVector(-X, Y, -Z), FVector(X, Y, -Z), FVector(X, Y, Z), FVector(-X, Y, Z),
        // Back face (-Y)  
        FVector(X, -Y, -Z), FVector(-X, -Y, -Z), FVector(-X, -Y, Z), FVector(X, -Y, Z),
        // Right face (+X)
        FVector(X, Y, -Z), FVector(X, -Y, -Z), FVector(X, -Y, Z), FVector(X, Y, Z),
        // Left face (-X)
        FVector(-X, -Y, -Z), FVector(-X, Y, -Z), FVector(-X, Y, Z), FVector(-X, -Y, Z),
        // Top face (+Z)
        FVector(-X, Y, Z), FVector(X, Y, Z), FVector(X, -Y, Z), FVector(-X, -Y, Z),
        // Bottom face (-Z)
        FVector(-X, -Y, -Z), FVector(X, -Y, -Z), FVector(X, Y, -Z), FVector(-X, Y, -Z)
    };

    TArray<FVector> Normals = {
        // Front (+Y), Back (-Y), Right (+X), Left (-X), Top (+Z), Bottom (-Z)
        FVector::Forward, FVector::Forward, FVector::Forward, FVector::Forward,
        FVector::Back, FVector::Back, FVector::Back, FVector::Back,
        FVector::Right, FVector::Right, FVector::Right, FVector::Right,
        FVector::Left, FVector::Left, FVector::Left, FVector::Left,
        FVector::Up, FVector::Up, FVector::Up, FVector::Up,
        FVector::Down, FVector::Down, FVector::Down, FVector::Down
    };

    TArray<FVector> UVs = {
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0),
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0),
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0),
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0),
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0),
        FVector(0,1,0), FVector(1,1,0), FVector(1,0,0), FVector(0,0,0)
    };

    // 정점 생성
    for (int32 i = 0; i < 24; ++i)
    {
        Vertices.push_back(FStaticMeshVertex(Positions[i], Normals[i], UVs[i].X, UVs[i].Y));
    }

    // 인덱스 생성 (시계방향 와인딩, 왼손 좌표계)
    TArray<uint32> FaceIndices = {
        0,1,2, 0,2,3,   // Front
        4,5,6, 4,6,7,   // Back  
        8,9,10, 8,10,11, // Right
        12,13,14, 12,14,15, // Left
        16,17,18, 16,18,19, // Top
        20,21,22, 20,22,23  // Bottom
    };
    
    Indices = FaceIndices;
    
    return FStaticMeshRenderData(Vertices, Indices);
}

FStaticMeshRenderData UKismetProceduralMeshLibrary::CreateSphereMesh(float SphereRadius, int32 SphereSegments, int32 SphereRings)
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;

    // UV 구 메쉬 생성 (Z-Up)
    for (int32 Ring = 0; Ring <= SphereRings; ++Ring)
    {
        float Phi = static_cast<float>(Ring) * PI / static_cast<float>(SphereRings);
        float Z = SphereRadius * cos(Phi);
        float RingRadius = SphereRadius * sin(Phi);

        for (int32 Segment = 0; Segment <= SphereSegments; ++Segment)
        {
            float Theta = static_cast<float>(Segment) * 2.0f * PI / static_cast<float>(SphereSegments);
            float X = RingRadius * cos(Theta);
            float Y = RingRadius * sin(Theta);

            FVector Position(X, Y, Z);
            FVector Normal = Position.Normalize();
            
            float U = static_cast<float>(Segment) / static_cast<float>(SphereSegments);
            float V = static_cast<float>(Ring) / static_cast<float>(SphereRings);

            Vertices.push_back(FStaticMeshVertex(Position, Normal, U, V));
        }
    }

    // 인덱스 생성 (왼손 좌표계 시계방향)
    for (int32 Ring = 0; Ring < SphereRings; ++Ring)
    {
        for (int32 Segment = 0; Segment < SphereSegments; ++Segment)
        {
            int32 Current = Ring * (SphereSegments + 1) + Segment;
            int32 Next = Current + SphereSegments + 1;

            // 첫 번째 삼각형 (시계방향)
            Indices.push_back(Current);
            Indices.push_back(Current + 1);
            Indices.push_back(Next);

            // 두 번째 삼각형 (시계방향)
            Indices.push_back(Current + 1);
            Indices.push_back(Next + 1);
            Indices.push_back(Next);
        }
    }

    return FStaticMeshRenderData(Vertices, Indices);
}

FStaticMeshRenderData UKismetProceduralMeshLibrary::CreateCylinderMesh(float CylinderRadius, float CylinderHeight, int32 CylinderSegments)
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;

    float HalfHeight = CylinderHeight * 0.5f;

    // 중심점들
    Vertices.push_back(FStaticMeshVertex(FVector(0.0f, 0.0f, HalfHeight), FVector::Up, 0.5f, 0.5f));  // 상단 중심
    Vertices.push_back(FStaticMeshVertex(FVector(0.0f, 0.0f, -HalfHeight), FVector::Down, 0.5f, 0.5f)); // 하단 중심

    // 측면 및 캡 정점들
    for (int32 i = 0; i <= CylinderSegments; ++i)
    {
        float Theta = static_cast<float>(i) * 2.0f * PI / static_cast<float>(CylinderSegments);
        float X = CylinderRadius * cos(Theta);
        float Y = CylinderRadius * sin(Theta);

        FVector SideNormal(X / CylinderRadius, Y / CylinderRadius, 0.0f);
        float U = static_cast<float>(i) / static_cast<float>(CylinderSegments);

        // 측면 정점들
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, HalfHeight), SideNormal, U, 0.0f));   // 상단
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, -HalfHeight), SideNormal, U, 1.0f));  // 하단

        // 캡 정점들
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, HalfHeight), FVector::Up, 
                                           0.5f + 0.5f * X / CylinderRadius, 0.5f + 0.5f * Y / CylinderRadius));
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, -HalfHeight), FVector::Down, 
                                           0.5f + 0.5f * X / CylinderRadius, 0.5f - 0.5f * Y / CylinderRadius));
    }

    // 인덱스 생성
    uint32 BaseIndex = 2;
    
    // 측면
    for (int32 i = 0; i < CylinderSegments; ++i)
    {
        uint32 TopCurrent = BaseIndex + i * 4;
        uint32 BottomCurrent = BaseIndex + i * 4 + 1;
        uint32 TopNext = BaseIndex + ((i + 1) % (CylinderSegments + 1)) * 4;
        uint32 BottomNext = BaseIndex + ((i + 1) % (CylinderSegments + 1)) * 4 + 1;

        // 시계방향 와인딩
        Indices.push_back(TopCurrent);
        Indices.push_back(TopNext);
        Indices.push_back(BottomCurrent);

        Indices.push_back(TopNext);
        Indices.push_back(BottomNext);
        Indices.push_back(BottomCurrent);
    }

    // 상단 캡 (시계방향)
    for (int32 i = 0; i < CylinderSegments; ++i)
    {
        uint32 CapCurrent = BaseIndex + i * 4 + 2;
        uint32 CapNext = BaseIndex + ((i + 1) % (CylinderSegments + 1)) * 4 + 2;

        Indices.push_back(0);
        Indices.push_back(CapNext);
        Indices.push_back(CapCurrent);
    }

    // 하단 캡 (시계방향)
    for (int32 i = 0; i < CylinderSegments; ++i)
    {
        uint32 CapCurrent = BaseIndex + i * 4 + 3;
        uint32 CapNext = BaseIndex + ((i + 1) % (CylinderSegments + 1)) * 4 + 3;

        Indices.push_back(1);
        Indices.push_back(CapCurrent);
        Indices.push_back(CapNext);
    }

    return FStaticMeshRenderData(Vertices, Indices);
}

FStaticMeshRenderData UKismetProceduralMeshLibrary::CreateConeMesh(float ConeRadius, float ConeHeight, int32 ConeSegments)
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;

    float HalfHeight = ConeHeight * 0.5f;

    // 꼭짓점과 하단 중심
    Vertices.push_back(FStaticMeshVertex(FVector(0.0f, 0.0f, HalfHeight), FVector::Up, 0.5f, 0.0f));      // 꼭짓점
    Vertices.push_back(FStaticMeshVertex(FVector(0.0f, 0.0f, -HalfHeight), FVector::Down, 0.5f, 0.5f));   // 하단 중심

    // 측면 법선 계산
    float SideLength = sqrt(ConeRadius * ConeRadius + ConeHeight * ConeHeight);
    float NormalY = ConeRadius / SideLength;
    float NormalXZ = ConeHeight / SideLength;

    // 하단 링과 측면 정점들
    for (int32 i = 0; i <= ConeSegments; ++i)
    {
        float Theta = static_cast<float>(i) * 2.0f * PI / static_cast<float>(ConeSegments);
        float X = ConeRadius * cos(Theta);
        float Y = ConeRadius * sin(Theta);

        FVector SideNormal(NormalXZ * X / ConeRadius, NormalXZ * Y / ConeRadius, NormalY);
        float U = static_cast<float>(i) / static_cast<float>(ConeSegments);

        // 측면 정점
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, -HalfHeight), SideNormal, U, 1.0f));

        // 하단 캡 정점
        Vertices.push_back(FStaticMeshVertex(FVector(X, Y, -HalfHeight), FVector::Down, 
                                           0.5f + 0.5f * X / ConeRadius, 0.5f - 0.5f * Y / ConeRadius));
    }

    // 측면 인덱스 (시계방향)
    uint32 BaseIndex = 2;
    for (int32 i = 0; i < ConeSegments; ++i)
    {
        uint32 BottomCurrent = BaseIndex + i * 2;
        uint32 BottomNext = BaseIndex + ((i + 1) % (ConeSegments + 1)) * 2;

        Indices.push_back(0); // 꼭짓점
        Indices.push_back(BottomNext);
        Indices.push_back(BottomCurrent);
    }

    // 하단 캡 (시계방향)
    for (int32 i = 0; i < ConeSegments; ++i)
    {
        uint32 CapCurrent = BaseIndex + i * 2 + 1;
        uint32 CapNext = BaseIndex + ((i + 1) % (ConeSegments + 1)) * 2 + 1;

        Indices.push_back(1); // 하단 중심
        Indices.push_back(CapCurrent);
        Indices.push_back(CapNext);
    }

    return FStaticMeshRenderData(Vertices, Indices);
}

FStaticMeshRenderData UKismetProceduralMeshLibrary::CreatePlaneMesh(FVector PlaneSize, int32 WidthSegments, int32 HeightSegments)
{
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;

    float Width = PlaneSize.X;
    float Height = PlaneSize.Y;
    float HalfWidth = Width * 0.5f;
    float HalfHeight = Height * 0.5f;

    // 정점 생성 (Z-Up 평면)
    for (int32 y = 0; y <= HeightSegments; ++y)
    {
        for (int32 x = 0; x <= WidthSegments; ++x)
        {
            float XPos = (static_cast<float>(x) / static_cast<float>(WidthSegments) - 0.5f) * Width;
            float YPos = (static_cast<float>(y) / static_cast<float>(HeightSegments) - 0.5f) * Height;

            FVector Position(XPos, YPos, 0.0f);
            FVector Normal = FVector::Up;
            
            float U = static_cast<float>(x) / static_cast<float>(WidthSegments);
            float V = 1.0f - static_cast<float>(y) / static_cast<float>(HeightSegments);

            Vertices.push_back(FStaticMeshVertex(Position, Normal, U, V));
        }
    }

    // 인덱스 생성 (시계방향)
    for (int32 y = 0; y < HeightSegments; ++y)
    {
        for (int32 x = 0; x < WidthSegments; ++x)
        {
            uint32 BottomLeft = y * (WidthSegments + 1) + x;
            uint32 BottomRight = BottomLeft + 1;
            uint32 TopLeft = (y + 1) * (WidthSegments + 1) + x;
            uint32 TopRight = TopLeft + 1;

            // 첫 번째 삼각형 (시계방향)
            Indices.push_back(BottomLeft);
            Indices.push_back(TopRight);
            Indices.push_back(TopLeft);

            // 두 번째 삼각형 (시계방향)
            Indices.push_back(BottomLeft);
            Indices.push_back(BottomRight);
            Indices.push_back(TopRight);
        }
    }

    return FStaticMeshRenderData(Vertices, Indices);
}