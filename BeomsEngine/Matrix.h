#pragma once
#include "Vector.h"
#include "Vector4.h"
#include <cmath>

struct FMatrix
{
public:
    float M[4][4];

    FMatrix()
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                M[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    FMatrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
    {
        M[0][0] = m00; M[0][1] = m01; M[0][2] = m02; M[0][3] = m03;
        M[1][0] = m10; M[1][1] = m11; M[1][2] = m12; M[1][3] = m13;
        M[2][0] = m20; M[2][1] = m21; M[2][2] = m22; M[2][3] = m23;
        M[3][0] = m30; M[3][1] = m31; M[3][2] = m32; M[3][3] = m33;
    }

    static const FMatrix Identity;
    static const FMatrix Zero;

    FMatrix operator+(const FMatrix& Other) const
    {
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Result.M[i][j] = M[i][j] + Other.M[i][j];
            }
        }
        return Result;
    }

    FMatrix operator-(const FMatrix& Other) const
    {
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Result.M[i][j] = M[i][j] - Other.M[i][j];
            }
        }
        return Result;
    }

    FMatrix operator*(const FMatrix& Other) const
    {
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Result.M[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    Result.M[i][j] += M[i][k] * Other.M[k][j];
                }
            }
        }
        return Result;
    }

    FMatrix operator*(float Scalar) const
    {
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Result.M[i][j] = M[i][j] * Scalar;
            }
        }
        return Result;
    }

    FMatrix& operator+=(const FMatrix& Other)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                M[i][j] += Other.M[i][j];
            }
        }
        return *this;
    }

    FMatrix& operator-=(const FMatrix& Other)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                M[i][j] -= Other.M[i][j];
            }
        }
        return *this;
    }

    FMatrix& operator*=(const FMatrix& Other)
    {
        *this = *this * Other;
        return *this;
    }

    FMatrix& operator*=(float Scalar)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                M[i][j] *= Scalar;
            }
        }
        return *this;
    }

    bool operator==(const FMatrix& Other) const
    {
        const float Epsilon = 1e-6f;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (std::abs(M[i][j] - Other.M[i][j]) >= Epsilon)
                    return false;
            }
        }
        return true;
    }

    bool operator!=(const FMatrix& Other) const
    {
        return !(*this == Other);
    }

    FVector4 operator*(const FVector4& Vector) const
    {
        return FVector4(
            M[0][0] * Vector.X + M[0][1] * Vector.Y + M[0][2] * Vector.Z + M[0][3] * Vector.W,
            M[1][0] * Vector.X + M[1][1] * Vector.Y + M[1][2] * Vector.Z + M[1][3] * Vector.W,
            M[2][0] * Vector.X + M[2][1] * Vector.Y + M[2][2] * Vector.Z + M[2][3] * Vector.W,
            M[3][0] * Vector.X + M[3][1] * Vector.Y + M[3][2] * Vector.Z + M[3][3] * Vector.W
        );
    }

    FVector TransformPosition(const FVector& Position) const
    {
        FVector4 V4(Position, 1.0f);
        FVector4 Result = *this * V4;
        return Result.ToVector3D();
    }

    FVector TransformDirection(const FVector& Direction) const
    {
        FVector4 V4(Direction, 0.0f);
        FVector4 Result = *this * V4;
        return FVector(Result.X, Result.Y, Result.Z);
    }

    FMatrix Transpose() const
    {
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Result.M[i][j] = M[j][i];
            }
        }
        return Result;
    }

    void TransposeInPlace()
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = i + 1; j < 4; ++j)
            {
                float Temp = M[i][j];
                M[i][j] = M[j][i];
                M[j][i] = Temp;
            }
        }
    }

    float Determinant() const
    {
        return 
            M[0][0] * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - 
                      M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + 
                      M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1])) -
            M[0][1] * (M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - 
                      M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + 
                      M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0])) +
            M[0][2] * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - 
                      M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + 
                      M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0])) -
            M[0][3] * (M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - 
                      M[1][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) + 
                      M[1][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
    }

    FMatrix Inverse() const
    {
        float Det = Determinant();
        if (std::abs(Det) < 1e-6f)
            return FMatrix::Identity;

        float InvDet = 1.0f / Det;
        FMatrix Result;

        Result.M[0][0] = (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1])) * InvDet;
        Result.M[0][1] = (-M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) + M[0][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[0][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1])) * InvDet;
        Result.M[0][2] = (M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) - M[0][2] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) + M[0][3] * (M[1][1] * M[3][2] - M[1][2] * M[3][1])) * InvDet;
        Result.M[0][3] = (-M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) + M[0][2] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) - M[0][3] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])) * InvDet;

        Result.M[1][0] = (-M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) + M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) - M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0])) * InvDet;
        Result.M[1][1] = (M[0][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[0][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[0][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0])) * InvDet;
        Result.M[1][2] = (-M[0][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) + M[0][2] * (M[1][0] * M[3][3] - M[1][3] * M[3][0]) - M[0][3] * (M[1][0] * M[3][2] - M[1][2] * M[3][0])) * InvDet;
        Result.M[1][3] = (M[0][0] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) - M[0][2] * (M[1][0] * M[2][3] - M[1][3] * M[2][0]) + M[0][3] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])) * InvDet;

        Result.M[2][0] = (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0])) * InvDet;
        Result.M[2][1] = (-M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + M[0][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) - M[0][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0])) * InvDet;
        Result.M[2][2] = (M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) - M[0][1] * (M[1][0] * M[3][3] - M[1][3] * M[3][0]) + M[0][3] * (M[1][0] * M[3][1] - M[1][1] * M[3][0])) * InvDet;
        Result.M[2][3] = (-M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) + M[0][1] * (M[1][0] * M[2][3] - M[1][3] * M[2][0]) - M[0][3] * (M[1][0] * M[2][1] - M[1][1] * M[2][0])) * InvDet;

        Result.M[3][0] = (-M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) + M[1][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) - M[1][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0])) * InvDet;
        Result.M[3][1] = (M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - M[0][1] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]) + M[0][2] * (M[2][0] * M[3][1] - M[2][1] * M[3][0])) * InvDet;
        Result.M[3][2] = (-M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) + M[0][1] * (M[1][0] * M[3][2] - M[1][2] * M[3][0]) - M[0][2] * (M[1][0] * M[3][1] - M[1][1] * M[3][0])) * InvDet;
        Result.M[3][3] = (M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]) + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0])) * InvDet;

        return Result;
    }

    bool InverseInPlace()
    {
        FMatrix Inv = Inverse();
        if (Inv == FMatrix::Identity && *this != FMatrix::Identity)
            return false;
        
        *this = Inv;
        return true;
    }

    static FMatrix CreateTranslation(const FVector& Translation)
    {
        return FMatrix(
            1.0f, 0.0f, 0.0f, Translation.X,
            0.0f, 1.0f, 0.0f, Translation.Y,
            0.0f, 0.0f, 1.0f, Translation.Z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static FMatrix CreateScale(const FVector& Scale)
    {
        return FMatrix(
            Scale.X, 0.0f, 0.0f, 0.0f,
            0.0f, Scale.Y, 0.0f, 0.0f,
            0.0f, 0.0f, Scale.Z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static FMatrix CreateScale(float UniformScale)
    {
        return CreateScale(FVector(UniformScale, UniformScale, UniformScale));
    }

    static FMatrix CreateRotationX(float RadianAngle)
    {
        float Cos = std::cos(RadianAngle);
        float Sin = std::sin(RadianAngle);
        
        return FMatrix(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, Cos, -Sin, 0.0f,
            0.0f, Sin, Cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static FMatrix CreateRotationY(float RadianAngle)
    {
        float Cos = std::cos(RadianAngle);
        float Sin = std::sin(RadianAngle);
        
        return FMatrix(
            Cos, 0.0f, Sin, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -Sin, 0.0f, Cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static FMatrix CreateRotationZ(float RadianAngle)
    {
        float Cos = std::cos(RadianAngle);
        float Sin = std::sin(RadianAngle);
        
        return FMatrix(
            Cos, -Sin, 0.0f, 0.0f,
            Sin, Cos, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static FMatrix CreateRotationFromEuler(float Roll, float Pitch, float Yaw)
    {
        return CreateRotationZ(Roll) * CreateRotationX(Pitch) * CreateRotationY(Yaw);
    }

    static FMatrix CreateRotationFromAxisAngle(const FVector& Axis, float RadianAngle)
    {
        FVector NormalizedAxis = Axis.Normalize();
        float Cos = std::cos(RadianAngle);
        float Sin = std::sin(RadianAngle);
        float OneMinusCos = 1.0f - Cos;

        float x = NormalizedAxis.X;
        float y = NormalizedAxis.Y;
        float z = NormalizedAxis.Z;

        return FMatrix(
            Cos + x * x * OneMinusCos,     x * y * OneMinusCos - z * Sin, x * z * OneMinusCos + y * Sin, 0.0f,
            y * x * OneMinusCos + z * Sin, Cos + y * y * OneMinusCos,     y * z * OneMinusCos - x * Sin, 0.0f,
            z * x * OneMinusCos - y * Sin, z * y * OneMinusCos + x * Sin, Cos + z * z * OneMinusCos,     0.0f,
            0.0f,                          0.0f,                          0.0f,                          1.0f
        );
    }

    static FMatrix CreateLookAt(const FVector& Eye, const FVector& Target, const FVector& Up)
    {
        FVector Forward = (Target - Eye).Normalize();
        FVector Right = Forward.Cross(Up).Normalize();
        FVector UpNormalized = Right.Cross(Forward);

        Forward = -Forward;

        FMatrix Result(
            Right.X, Right.Y, Right.Z, -Right.Dot(Eye),
            UpNormalized.X, UpNormalized.Y, UpNormalized.Z, -UpNormalized.Dot(Eye),
            Forward.X, Forward.Y, Forward.Z, -Forward.Dot(Eye),
            0.0f, 0.0f, 0.0f, 1.0f
        );

        return Result;
    }

    static FMatrix CreatePerspective(float FieldOfViewY, float AspectRatio, float NearPlane, float FarPlane)
    {
        float TanHalfFovy = std::tan(FieldOfViewY * 0.5f);
        float Range = FarPlane - NearPlane;

        return FMatrix(
            1.0f / (AspectRatio * TanHalfFovy), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / TanHalfFovy, 0.0f, 0.0f,
            0.0f, 0.0f, (FarPlane) / Range, (NearPlane * FarPlane) / Range,
            0.0f, 0.0f, -1.0f, 0.0f
        );
    }

    static FMatrix CreateOrthographic(float Width, float Height, float NearPlane, float FarPlane, float OrthographicScale)
    {
        float Range = FarPlane - NearPlane;

        return FMatrix(
            2.0f / Width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / Height, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f / Range, NearPlane / (-1 * Range),
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    float* operator[](int Index)
    {
        return M[Index];
    }

    const float* operator[](int Index) const
    {
        return M[Index];
    }
};

inline FMatrix operator*(float Scalar, const FMatrix& Matrix)
{
    return Matrix * Scalar;
}