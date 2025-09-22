#pragma once
#include <cmath>
#include "Vector.h"
#include "Vector2.h"

struct FVertex
{
    FVector Position;
    FVector Normal;
    FVector Tangent;
    FVector Binormal;
    FVector2 UV; // UV coordinates

    FVertex()
        : Position(FVector::Zero)
        , Normal(FVector::Up)
        , Tangent(FVector::Right)
        , Binormal(FVector::Forward)
        , UV(FVector2::Zero())
    {}

    FVertex(const FVector& InPosition, const FVector& InNormal, float InU, float InV)
        : Position(InPosition)
        , Normal(InNormal)
        , UV(FVector2(InU, InV))
    {
        // 접선 벡터 계산
        CalculateTangentSpace();
    }

    FVertex(const FVector& InPosition, const FVector& InNormal, const FVector& InTangent, float InU, float InV)
        : Position(InPosition)
        , Normal(InNormal)
        , Tangent(InTangent)
        , UV(FVector2(InU, InV))
    {
        // 법선과 접선으로부터 종법선 계산 (왼손 좌표계)
        Binormal = Normal.Cross(Tangent);
    }

private:
    void CalculateTangentSpace()
    {
        // 기본 접선 벡터 생성
        FVector UpVector = FVector::Up;
        if (std::abs(Normal.Dot(UpVector)) > 0.9f)
        {
            UpVector = FVector::Right;
        }
        
        Tangent = UpVector.Cross(Normal).Normalize();
        Binormal = Normal.Cross(Tangent);
    }
};