#pragma once
#include <corecrt_math.h>

struct FVector2
{
    float X;
    float Y;

    FVector2() : X(0.0f), Y(0.0f) {}

    FVector2(float InX, float InY) : X(InX), Y(InY) {}

    FVector2(const FVector2& Other) : X(Other.X), Y(Other.Y) {}

    FVector2& operator=(const FVector2& Other)
    {
        X = Other.X;
        Y = Other.Y;
        return *this;
    }

    FVector2 operator+(const FVector2& Other) const
    {
        return FVector2(X + Other.X, Y + Other.Y);
    }

    FVector2 operator-(const FVector2& Other) const
    {
        return FVector2(X - Other.X, Y - Other.Y);
    }

    FVector2 operator*(float Scalar) const
    {
        return FVector2(X * Scalar, Y * Scalar);
    }

    FVector2 operator/(float Scalar) const
    {
        return FVector2(X / Scalar, Y / Scalar);
    }

    FVector2& operator+=(const FVector2& Other)
    {
        X += Other.X;
        Y += Other.Y;
        return *this;
    }

    FVector2& operator-=(const FVector2& Other)
    {
        X -= Other.X;
        Y -= Other.Y;
        return *this;
    }

    FVector2& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        return *this;
    }

    FVector2& operator/=(float Scalar)
    {
        X /= Scalar;
        Y /= Scalar;
        return *this;
    }

    bool operator==(const FVector2& Other) const
    {
        return X == Other.X && Y == Other.Y;
    }

    bool operator!=(const FVector2& Other) const
    {
        return !(*this == Other);
    }

    float Dot(const FVector2& Other) const
    {
        return X * Other.X + Y * Other.Y;
    }

    float Length() const
    {
        return sqrtf(X * X + Y * Y);
    }

    float LengthSquared() const
    {
        return X * X + Y * Y;
    }

    FVector2 Normalize() const
    {
        float Len = Length();
        if (Len > 0.0f)
        {
            return *this / Len;
        }
        return FVector2(0.0f, 0.0f);
    }

    void NormalizeInPlace()
    {
        float Len = Length();
        if (Len > 0.0f)
        {
            *this /= Len;
        }
        else
        {
            X = Y = 0.0f;
        }
    }

    static FVector2 Zero() { return FVector2(0.0f, 0.0f); }
    static FVector2 One() { return FVector2(1.0f, 1.0f); }
    static FVector2 UnitX() { return FVector2(1.0f, 0.0f); }
    static FVector2 UnitY() { return FVector2(0.0f, 1.0f); }
};