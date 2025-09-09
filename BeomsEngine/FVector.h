#pragma once
#include "pch.h"

struct FVector
{
public:
    float X, Y, Z;

    // 생성자들
    FVector() : X(0.0f), Y(0.0f), Z(0.0f) {}
    FVector(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}
    FVector(float InValue) : X(InValue), Y(InValue), Z(InValue) {}

    // 정적 상수 벡터들 (Z-UP 왼손 좌표계)
    static const FVector Zero;      // (0, 0, 0)
    static const FVector One;       // (1, 1, 1)
    static const FVector Forward;   // (0, 1, 0) - Y축 양의 방향
    static const FVector Right;     // (1, 0, 0) - X축 양의 방향
    static const FVector Up;        // (0, 0, 1) - Z축 양의 방향
    static const FVector Back;      // (0, -1, 0) - Y축 음의 방향
    static const FVector Left;      // (-1, 0, 0) - X축 음의 방향
    static const FVector Down;      // (0, 0, -1) - Z축 음의 방향

    // 기본 연산자들
    FVector operator+(const FVector& Other) const
    {
        return FVector(X + Other.X, Y + Other.Y, Z + Other.Z);
    }

    FVector operator-(const FVector& Other) const
    {
        return FVector(X - Other.X, Y - Other.Y, Z - Other.Z);
    }

    FVector operator*(float Scalar) const
    {
        return FVector(X * Scalar, Y * Scalar, Z * Scalar);
    }

    FVector operator/(float Scalar) const
    {
        if (Scalar != 0.0f)
            return FVector(X / Scalar, Y / Scalar, Z / Scalar);
        return FVector::Zero;
    }

    FVector operator-() const
    {
        return FVector(-X, -Y, -Z);
    }

    // 복합 대입 연산자들
    FVector& operator+=(const FVector& Other)
    {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }

    FVector& operator-=(const FVector& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        return *this;
    }

    FVector& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this;
    }

    FVector& operator/=(float Scalar)
    {
        if (Scalar != 0.0f)
        {
            X /= Scalar;
            Y /= Scalar;
            Z /= Scalar;
        }
        return *this;
    }

    // 비교 연산자들
    bool operator==(const FVector& Other) const
    {
        const float Epsilon = 1e-6f;
        return (std::abs(X - Other.X) < Epsilon) &&
            (std::abs(Y - Other.Y) < Epsilon) &&
            (std::abs(Z - Other.Z) < Epsilon);
    }

    bool operator!=(const FVector& Other) const
    {
        return !(*this == Other);
    }

    // 벡터 연산 메서드들
    float Magnitude() const
    {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    float MagnitudeSquared() const
    {
        return X * X + Y * Y + Z * Z;
    }

    FVector Normalize() const
    {
        float Mag = Magnitude();
        if (Mag > 0.0f)
            return *this / Mag;
        return FVector::Zero;
    }

    void NormalizeInPlace()
    {
        *this = Normalize();
    }

    bool IsZero() const
    {
        const float Epsilon = 1e-6f;
        return (std::abs(X) < Epsilon) && (std::abs(Y) < Epsilon) && (std::abs(Z) < Epsilon);
    }

    bool IsNormalized() const
    {
        const float Epsilon = 1e-4f;
        return std::abs(MagnitudeSquared() - 1.0f) < Epsilon;
    }

    float Dot(const FVector& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    // 외적 (Cross Product)
    FVector Cross(const FVector& Other) const
    {
        return FVector(
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X
        );
    }

    // 거리 계산
    float Distance(const FVector& Other) const
    {
        return (*this - Other).Magnitude();
    }

    float DistanceSquared(const FVector& Other) const
    {
        return (*this - Other).MagnitudeSquared();
    }

    // 선형 보간
    static FVector Lerp(const FVector& A, const FVector& B, float Alpha)
    {
        return A + (B - A) * Alpha;
    }

    // 두 벡터 사이의 각도 (라디안)
    float AngleBetween(const FVector& Other) const
    {
        float DotProduct = Dot(Other);
        float Magnitudes = Magnitude() * Other.Magnitude();

        if (Magnitudes > 0.0f)
        {
            float CosAngle = DotProduct / Magnitudes;
            // Clamp to avoid floating point errors
            CosAngle = std::max(-1.0f, std::min(1.0f, CosAngle));
            return std::acos(CosAngle);
        }
        return 0.0f;
    }

    // 벡터 투영
    FVector Project(const FVector& OnNormal) const
    {
        float DotProduct = Dot(OnNormal);
        float NormalMagSquared = OnNormal.MagnitudeSquared();

        if (NormalMagSquared > 0.0f)
            return OnNormal * (DotProduct / NormalMagSquared);
        return FVector::Zero;
    }

    // 배열 접근자
    float& operator[](int Index)
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        default: return X; // 안전장치
        }
    }

    const float& operator[](int Index) const
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        default: return X; // 안전장치
        }
    }
};

const FVector FVector::Zero(0.0f, 0.0f, 0.0f);
const FVector FVector::One(1.0f, 1.0f, 1.0f);
const FVector FVector::Forward(0.0f, 1.0f, 0.0f);   
const FVector FVector::Right(1.0f, 0.0f, 0.0f);     
const FVector FVector::Up(0.0f, 0.0f, 1.0f);        
const FVector FVector::Back(0.0f, -1.0f, 0.0f);     
const FVector FVector::Left(-1.0f, 0.0f, 0.0f);     
const FVector FVector::Down(0.0f, 0.0f, -1.0f);     

// 스칼라 * 벡터 연산자
FVector operator*(float Scalar, const FVector& Vector)
{
    return Vector * Scalar;
}