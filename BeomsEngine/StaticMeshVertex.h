#pragma once
#include "Vector.h"
#include <cmath>

struct FStaticMeshVertex
{
    FVector Position;
    FVector Normal;
    FVector Tangent;
    FVector Binormal;
    float U, V; // UV coordinates

    FStaticMeshVertex() 
        : Position(FVector::Zero)
        , Normal(FVector::Up)
        , Tangent(FVector::Right)
        , Binormal(FVector::Forward)
        , U(0.0f)
        , V(0.0f)
    {}

    FStaticMeshVertex(const FVector& InPosition, const FVector& InNormal, float InU, float InV)
        : Position(InPosition)
        , Normal(InNormal)
        , U(InU)
        , V(InV)
    {
        // Z-Up 왼손 좌표계에서 접선 벡터 계산
        CalculateTangentSpace();
    }

    FStaticMeshVertex(const FVector& InPosition, const FVector& InNormal, const FVector& InTangent, float InU, float InV)
        : Position(InPosition)
        , Normal(InNormal)
        , Tangent(InTangent)
        , U(InU)
        , V(InV)
    {
        // 법선과 접선으로부터 종법선 계산 (왼손 좌표계)
        Binormal = Normal.Cross(Tangent);
    }

private:
    void CalculateTangentSpace()
    {
        // Z-Up 왼손 좌표계에서 기본 접선 벡터 생성
        FVector UpVector = FVector::Up;
        if (std::abs(Normal.Dot(UpVector)) > 0.9f)
        {
            UpVector = FVector::Right;
        }
        
        Tangent = UpVector.Cross(Normal).Normalize();
        Binormal = Normal.Cross(Tangent);
    }
};