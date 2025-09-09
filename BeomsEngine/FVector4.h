#pragma once
#include "FVector.h"
#include <cmath>

struct FVector4
{
public:
    float X, Y, Z, W;

    FVector4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}
    FVector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
    FVector4(float InValue) : X(InValue), Y(InValue), Z(InValue), W(InValue) {}
	explicit FVector4(const FVector& InVector, float InW = 1.0f) : X(InVector.X), Y(InVector.Y), Z(InVector.Z), W(InW) {}

    static const FVector4 Zero;     // (0, 0, 0, 0)
    static const FVector4 One;      // (1, 1, 1, 1)
    static const FVector4 UnitX;    // (1, 0, 0, 0)
    static const FVector4 UnitY;    // (0, 1, 0, 0)
    static const FVector4 UnitZ;    // (0, 0, 1, 0)
    static const FVector4 UnitW;    // (0, 0, 0, 1)
    static const FVector4 Point;    // (0, 0, 0, 1)
    static const FVector4 Vector;   // (0, 0, 0, 0)

    FVector4 operator+(const FVector4& Other) const
    {
        return FVector4(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
    }

    FVector4 operator-(const FVector4& Other) const
    {
        return FVector4(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
    }

    FVector4 operator*(float Scalar) const
    {
        return FVector4(X * Scalar, Y * Scalar, Z * Scalar, W * Scalar);
    }

    FVector4 operator/(float Scalar) const
    {
        if (Scalar != 0.0f)
            return FVector4(X / Scalar, Y / Scalar, Z / Scalar, W / Scalar);
        return FVector4::Zero;
    }

    FVector4 operator-() const
    {
        return FVector4(-X, -Y, -Z, -W);
    }

    FVector4& operator+=(const FVector4& Other)
    {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        W += Other.W;
        return *this;
    }

    FVector4& operator-=(const FVector4& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        W -= Other.W;
        return *this;
    }

    FVector4& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        W *= Scalar;
        return *this;
    }

    FVector4& operator/=(float Scalar)
    {
        if (Scalar != 0.0f)
        {
            X /= Scalar;
            Y /= Scalar;
            Z /= Scalar;
            W /= Scalar;
        }
        return *this;
    }

    bool operator==(const FVector4& Other) const
    {
        const float Epsilon = 1e-6f;
        return (std::abs(X - Other.X) < Epsilon) &&
            (std::abs(Y - Other.Y) < Epsilon) &&
            (std::abs(Z - Other.Z) < Epsilon) &&
            (std::abs(W - Other.W) < Epsilon);
    }

    bool operator!=(const FVector4& Other) const
    {
        return !(*this == Other);
    }

    float Magnitude() const
    {
        return std::sqrt(X * X + Y * Y + Z * Z + W * W);
    }

    float MagnitudeSquared() const
    {
        return X * X + Y * Y + Z * Z + W * W;
    }

    float Magnitude3D() const
    {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    float MagnitudeSquared3D() const
    {
        return X * X + Y * Y + Z * Z;
    }

    FVector4 Normalize() const
    {
        float Mag = Magnitude();
        if (Mag > 0.0f)
            return *this / Mag;
        return FVector4::Zero;
    }

    FVector4 Normalize3D() const
    {
        float Mag3D = Magnitude3D();
        if (Mag3D > 0.0f)
            return FVector4(X / Mag3D, Y / Mag3D, Z / Mag3D, W);
        return FVector4(0.0f, 0.0f, 0.0f, W);
    }

    void NormalizeInPlace()
    {
        *this = Normalize();
    }

    void Normalize3DInPlace()
    {
        *this = Normalize3D();
    }

    bool IsZero() const
    {
        const float Epsilon = 1e-6f;
        return (std::abs(X) < Epsilon) && (std::abs(Y) < Epsilon) &&
            (std::abs(Z) < Epsilon) && (std::abs(W) < Epsilon);
    }

    bool IsNormalized() const
    {
        const float Epsilon = 1e-4f;
        return std::abs(MagnitudeSquared() - 1.0f) < Epsilon;
    }

    float Dot(const FVector4& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
    }

    float Dot3D(const FVector4& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    // 벡터곱 (외적) - 3D 부분만 사용
    FVector4 Cross3D(const FVector4& Other) const
    {
        return FVector4(
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X,
            0.0f // 벡터이므로 W=0
        );
    }

    FVector ToVector3D() const
    {
        if (std::abs(W) > 1e-6f && std::abs(W - 1.0f) > 1e-6f)
            return FVector(X / W, Y / W, Z / W);
        return FVector(X, Y, Z);
    }

    bool IsPoint() const
    {
        const float Epsilon = 1e-4f;
        return std::abs(W - 1.0f) < Epsilon;
    }

    bool IsVector() const
    {
        const float Epsilon = 1e-6f;
        return std::abs(W) < Epsilon;
    }

	// 동차 좌표 정규화 (W를 1로 만듦)
    FVector4 HomogeneousNormalize() const
    {
        if (std::abs(W) > 1e-6f && std::abs(W - 1.0f) > 1e-6f)
            return FVector4(X / W, Y / W, Z / W, 1.0f);
        return *this;
    }

    void HomogeneousNormalizeInPlace()
    {
        *this = HomogeneousNormalize();
    }

    float Distance(const FVector4& Other) const
    {
        return (*this - Other).Magnitude();
    }

    float DistanceSquared(const FVector4& Other) const
    {
        return (*this - Other).MagnitudeSquared();
    }

    float Distance3D(const FVector4& Other) const
    {
        return (*this - Other).Magnitude3D();
    }

    float DistanceSquared3D(const FVector4& Other) const
    {
        return (*this - Other).MagnitudeSquared3D();
    }

    static FVector4 Lerp(const FVector4& A, const FVector4& B, float Alpha)
    {
        return A + (B - A) * Alpha;
    }

    float& operator[](int Index)
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        case 3: return W;
        default: return X; 
        }
    }

    const float& operator[](int Index) const
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        case 3: return W;
        default: return X;
        }
    }

    float& R() { return X; }
    float& G() { return Y; }
    float& B() { return Z; }
    float& A() { return W; }

    const float& R() const { return X; }
    const float& G() const { return Y; }
    const float& B() const { return Z; }
    const float& A() const { return W; }
};


inline FVector4 operator*(float Scalar, const FVector4& Vector)
{
    return Vector * Scalar;
}