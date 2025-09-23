#pragma once
#include "Math.h"
#include "Vertex.h"

// 바운딩 박스와 바운딩 스피어를 결합한 구조체
struct FBoxSphereBounds
{
    FVector Origin;        // 바운딩 박스 중심점
    FVector BoxExtent;     // 바운딩 박스 반 크기 (중심에서 각 축까지의 거리)
    float SphereRadius;    // 바운딩 스피어 반지름

    FBoxSphereBounds()
        : Origin(FVector::Zero)
        , BoxExtent(FVector::Zero)
        , SphereRadius(0.0f)
    {}

    FBoxSphereBounds(const FVector& InOrigin, const FVector& InBoxExtent, float InSphereRadius)
        : Origin(InOrigin)
        , BoxExtent(InBoxExtent)
        , SphereRadius(InSphereRadius)
    {}

    // 정점 배열로부터 바운딩 계산
    FBoxSphereBounds(const TArray<struct FVertex>& Vertices)
    {
        if (Vertices.empty())
        {
            *this = FBoxSphereBounds();
            return;
        }

        // 최소/최대 찾기
        FVector MinBounds = Vertices[0].Position;
        FVector MaxBounds = Vertices[0].Position;

        for (const auto& Vertex : Vertices)
        {
            MinBounds.X = fmin(MinBounds.X, Vertex.Position.X);
            MinBounds.Y = fmin(MinBounds.Y, Vertex.Position.Y);
            MinBounds.Z = fmin(MinBounds.Z, Vertex.Position.Z);

            MaxBounds.X = fmax(MaxBounds.X, Vertex.Position.X);
            MaxBounds.Y = fmax(MaxBounds.Y, Vertex.Position.Y);
            MaxBounds.Z = fmax(MaxBounds.Z, Vertex.Position.Z);
        }

        // 바운딩 박스 계산
        Origin = (MinBounds + MaxBounds) * 0.5f;
        BoxExtent = (MaxBounds - MinBounds) * 0.5f;

        // 바운딩 스피어 반지름 계산
        float MaxDistanceSquared = 0.0f;
        for (const auto& Vertex : Vertices)
        {
            FVector Diff = Vertex.Position - Origin;
            float DistanceSquared = Diff.X * Diff.X + Diff.Y * Diff.Y + Diff.Z * Diff.Z;
            MaxDistanceSquared = fmax(MaxDistanceSquared, DistanceSquared);
        }
        SphereRadius = sqrt(MaxDistanceSquared);
    }

    // 바운딩 박스 최소점 반환
    FVector GetBoxMin() const
    {
        return Origin - BoxExtent;
    }

    // 바운딩 박스 최대점 반환
    FVector GetBoxMax() const
    {
        return Origin + BoxExtent;
    }

    // 바운딩 박스 크기 반환
    FVector GetBoxSize() const
    {
        return BoxExtent * 2.0f;
    }

    // 다른 바운딩과 결합
    FBoxSphereBounds operator+(const FBoxSphereBounds& Other) const
    {
        if (SphereRadius <= 0.0f)
            return Other;
        if (Other.SphereRadius <= 0.0f)
            return *this;

        FVector NewMin = FVector(
            fmin(GetBoxMin().X, Other.GetBoxMin().X),
            fmin(GetBoxMin().Y, Other.GetBoxMin().Y),
            fmin(GetBoxMin().Z, Other.GetBoxMin().Z)
        );

        FVector NewMax = FVector(
            fmax(GetBoxMax().X, Other.GetBoxMax().X),
            fmax(GetBoxMax().Y, Other.GetBoxMax().Y),
            fmax(GetBoxMax().Z, Other.GetBoxMax().Z)
        );

        FVector NewOrigin = (NewMin + NewMax) * 0.5f;
        FVector NewBoxExtent = (NewMax - NewMin) * 0.5f;

        // 새로운 스피어 반지름 계산
        float NewSphereRadius = fmax(
            (Origin - NewOrigin).Magnitude() + SphereRadius,
            (Other.Origin - NewOrigin).Magnitude() + Other.SphereRadius
        );

        return FBoxSphereBounds(NewOrigin, NewBoxExtent, NewSphereRadius);
    }

    // 변환 적용
    FBoxSphereBounds TransformBy(const FMatrix& Transform) const
    {
        // TODO: 실제 매트릭스 변환 구현
        // 현재는 단순화된 버전
        return *this;
    }

    // 유효성 검사
    bool IsValid() const
    {
        return SphereRadius > 0.0f;
    }

    // 점이 바운딩 박스 안에 있는지 검사
    bool IsInsideBox(const FVector& Point) const
    {
        FVector Min = GetBoxMin();
        FVector Max = GetBoxMax();

        return Point.X >= Min.X && Point.X <= Max.X &&
               Point.Y >= Min.Y && Point.Y <= Max.Y &&
               Point.Z >= Min.Z && Point.Z <= Max.Z;
    }

    // 점이 바운딩 스피어 안에 있는지 검사
    bool IsInsideSphere(const FVector& Point) const
    {
        FVector Diff = Point - Origin;
        float DistanceSquared = Diff.X * Diff.X + Diff.Y * Diff.Y + Diff.Z * Diff.Z;
        return DistanceSquared <= (SphereRadius * SphereRadius);
    }

    // 다른 바운딩과 겹치는지 검사 (박스 기준)
    bool IntersectsBox(const FBoxSphereBounds& Other) const
    {
        FVector MyMin = GetBoxMin();
        FVector MyMax = GetBoxMax();
        FVector OtherMin = Other.GetBoxMin();
        FVector OtherMax = Other.GetBoxMax();

        return MyMin.X <= OtherMax.X && MyMax.X >= OtherMin.X &&
               MyMin.Y <= OtherMax.Y && MyMax.Y >= OtherMin.Y &&
               MyMin.Z <= OtherMax.Z && MyMax.Z >= OtherMin.Z;
    }

    // 다른 바운딩과 겹치는지 검사 (스피어 기준)
    bool IntersectsSphere(const FBoxSphereBounds& Other) const
    {
        FVector Diff = Origin - Other.Origin;
        float DistanceSquared = Diff.X * Diff.X + Diff.Y * Diff.Y + Diff.Z * Diff.Z;
        float CombinedRadius = SphereRadius + Other.SphereRadius;
        return DistanceSquared <= (CombinedRadius * CombinedRadius);
    }

    // 바운딩을 확장
    FBoxSphereBounds ExpandBy(float ExpandAmount) const
    {
        return FBoxSphereBounds(
            Origin,
            BoxExtent + FVector(ExpandAmount, ExpandAmount, ExpandAmount),
            SphereRadius + ExpandAmount
        );
    }

    // 바운딩을 특정 점으로 확장
    FBoxSphereBounds ExpandToInclude(const FVector& Point) const
    {
        FVector NewMin = FVector(
            fmin(GetBoxMin().X, Point.X),
            fmin(GetBoxMin().Y, Point.Y),
            fmin(GetBoxMin().Z, Point.Z)
        );

        FVector NewMax = FVector(
            fmax(GetBoxMax().X, Point.X),
            fmax(GetBoxMax().Y, Point.Y),
            fmax(GetBoxMax().Z, Point.Z)
        );

        FVector NewOrigin = (NewMin + NewMax) * 0.5f;
        FVector NewBoxExtent = (NewMax - NewMin) * 0.5f;

        FVector Diff = Point - NewOrigin;
        float DistanceToPoint = sqrt(Diff.X * Diff.X + Diff.Y * Diff.Y + Diff.Z * Diff.Z);
        float NewSphereRadius = fmax(SphereRadius, DistanceToPoint);

        return FBoxSphereBounds(NewOrigin, NewBoxExtent, NewSphereRadius);
    }
};