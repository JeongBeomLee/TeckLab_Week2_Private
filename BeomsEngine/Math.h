#pragma once
#include <cmath>
#include <algorithm>
#include <limits>
#include "Types.h"

// Unreal Engine 스타일 수학 유틸리티 클래스
class FMath
{
public:
    // 상수들
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float HALF_PI = PI * 0.5f;
    static constexpr float TWO_PI = PI * 2.0f;
    static constexpr float INV_PI = 1.0f / PI;
	static constexpr float PI_DIV_2 = PI / 2.0f;
	static constexpr float PI_DIV_4 = PI / 4.0f;
    static constexpr float SMALL_NUMBER = 1e-8f;
    static constexpr float KINDA_SMALL_NUMBER = 1e-4f;
    static constexpr float BIG_NUMBER = 3.4e+38f;
    static constexpr float EULERS_NUMBER = 2.71828182845904523536f;
    
    // 각도 변환
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;

    // 기본 수학 함수들
    template<typename T>
    static T Abs(T Value)
    {
        return Value < 0 ? -Value : Value;
    }

    template<typename T>
    static T Sign(T Value)
    {
        return Value > 0 ? T(1) : (Value < 0 ? T(-1) : T(0));
    }

    template<typename T>
    static T Min(T A, T B)
    {
        return A < B ? A : B;
    }

    template<typename T>
    static T Max(T A, T B)
    {
        return A > B ? A : B;
    }

    template<typename T>
    static T Min3(T A, T B, T C)
    {
        return Min(A, Min(B, C));
    }

    template<typename T>
    static T Max3(T A, T B, T C)
    {
        return Max(A, Max(B, C));
    }

    template<typename T>
    static T Clamp(T Value, T MinValue, T MaxValue)
    {
        return Value < MinValue ? MinValue : (Value > MaxValue ? MaxValue : Value);
    }

    template<typename T>
    static T Square(T Value)
    {
        return Value * Value;
    }

    // 부동소수점 수학 함수들
    static float Sqrt(float Value)
    {
        return std::sqrt(Value);
    }

    static double Sqrt(double Value)
    {
        return std::sqrt(Value);
    }

    static float InvSqrt(float Value)
    {
        return 1.0f / Sqrt(Value);
    }

    static float Pow(float Base, float Exponent)
    {
        return std::pow(Base, Exponent);
    }

    static double Pow(double Base, double Exponent)
    {
        return std::pow(Base, Exponent);
    }

    static float Exp(float Value)
    {
        return std::exp(Value);
    }

    static float Exp2(float Value)
    {
        return std::exp2(Value);
    }

    static float Log(float Value)
    {
        return std::log(Value);
    }

    static float LogX(float Base, float Value)
    {
        return Log(Value) / Log(Base);
    }

    static float Log2(float Value)
    {
        return std::log2(Value);
    }

    static float Log10(float Value)
    {
        return std::log10(Value);
    }

    // 삼각함수
    static float Sin(float Value)
    {
        return std::sin(Value);
    }

    static float Cos(float Value)
    {
        return std::cos(Value);
    }

    static float Tan(float Value)
    {
        return std::tan(Value);
    }

    static float Asin(float Value)
    {
        return std::asin(Clamp(Value, -1.0f, 1.0f));
    }

    static float Acos(float Value)
    {
        return std::acos(Clamp(Value, -1.0f, 1.0f));
    }

    static float Atan(float Value)
    {
        return std::atan(Value);
    }

    static float Atan2(float Y, float X)
    {
        return std::atan2(Y, X);
    }

    static void SinCos(float* ScalarSin, float* ScalarCos, float Value)
    {
        *ScalarSin = Sin(Value);
        *ScalarCos = Cos(Value);
    }

    // 각도 변환
    static float DegreesToRadians(float Degrees)
    {
        return Degrees * DEG_TO_RAD;
    }

    static float RadiansToDegrees(float Radians)
    {
        return Radians * RAD_TO_DEG;
    }

    // 반올림 함수들
    static float Floor(float Value)
    {
        return std::floor(Value);
    }

    static int32 FloorToInt(float Value)
    {
        return static_cast<int32>(Floor(Value));
    }

    static float Ceil(float Value)
    {
        return std::ceil(Value);
    }

    static int32 CeilToInt(float Value)
    {
        return static_cast<int32>(Ceil(Value));
    }

    static float Round(float Value)
    {
        return std::round(Value);
    }

    static int32 RoundToInt(float Value)
    {
        return static_cast<int32>(Round(Value));
    }

    static float Frac(float Value)
    {
        return Value - Floor(Value);
    }

    static float Modf(float Value, float* IntPart)
    {
        return std::modf(Value, IntPart);
    }

    static float Fmod(float X, float Y)
    {
        return std::fmod(X, Y);
    }

    // 보간 함수들
    template<typename T>
    static T Lerp(const T& A, const T& B, float Alpha)
    {
        return A + Alpha * (B - A);
    }

    template<typename T>
    static T LerpStable(const T& A, const T& B, float Alpha)
    {
        return (1.0f - Alpha) * A + Alpha * B;
    }

    static float SmoothStep(float A, float B, float X)
    {
        if (X < A) return 0.0f;
        if (X >= B) return 1.0f;
        const float InterpFraction = (X - A) / (B - A);
        return InterpFraction * InterpFraction * (3.0f - 2.0f * InterpFraction);
    }

    // 비교 함수들
    static bool IsNearlyEqual(float A, float B, float ErrorTolerance = KINDA_SMALL_NUMBER)
    {
        return Abs(A - B) <= ErrorTolerance;
    }

    static bool IsNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER)
    {
        return Abs(Value) <= ErrorTolerance;
    }

    static bool IsFinite(float Value)
    {
        return std::isfinite(Value);
    }

    static bool IsNaN(float Value)
    {
        return std::isnan(Value);
    }

    static bool IsInfinite(float Value)
    {
        return std::isinf(Value);
    }

    // 랜덤 함수들 (간단한 구현)
    static void RandInit(int32 Seed)
    {
        srand(static_cast<unsigned int>(Seed));
    }

    static int32 Rand()
    {
        return rand();
    }

    static float FRand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    static float RandRange(float Min, float Max)
    {
        return Min + (Max - Min) * FRand();
    }

    static int32 RandRange(int32 Min, int32 Max)
    {
        return Min + (rand() % (Max - Min + 1));
    }

    // 기타 유용한 함수들
    template<typename T>
    static void Swap(T& A, T& B)
    {
        T Temp = A;
        A = B;
        B = Temp;
    }

    static bool IsPowerOfTwo(uint32 Value)
    {
        return Value != 0 && (Value & (Value - 1)) == 0;
    }

    static uint32 RoundUpToPowerOfTwo(uint32 Value)
    {
        Value--;
        Value |= Value >> 1;
        Value |= Value >> 2;
        Value |= Value >> 4;
        Value |= Value >> 8;
        Value |= Value >> 16;
        Value++;
        return Value;
    }

    static float Wrap(float Value, float Min, float Max)
    {
        const float Range = Max - Min;
        if (Range <= 0.0f)
            return Min;

        const float Result = Fmod(Value - Min, Range);
        return Result < 0.0f ? Result + Max : Result + Min;
    }

    // 각도 정규화
    static float NormalizeAngle(float Angle)
    {
        // -180 to 180도 사이로 정규화
        Angle = Fmod(Angle + 180.0f, 360.0f);
        if (Angle < 0.0f)
            Angle += 360.0f;
        return Angle - 180.0f;
    }

    static float UnwindDegrees(float Angle)
    {
        while (Angle > 180.0f)
            Angle -= 360.0f;
        while (Angle < -180.0f)
            Angle += 360.0f;
        return Angle;
    }

    static float UnwindRadians(float Angle)
    {
        while (Angle > PI)
            Angle -= TWO_PI;
        while (Angle < -PI)
            Angle += TWO_PI;
        return Angle;
    }

    // 벡터 관련 유틸리티
    static float GetRangePct(float MinValue, float MaxValue, float Value)
    {
        return (Value - MinValue) / (MaxValue - MinValue);
    }

    static float GetRangeValue(float MinValue, float MaxValue, float Pct)
    {
        return Lerp(MinValue, MaxValue, Pct);
    }

    // 안전한 나눗셈
    template<typename T>
    static T SafeDivide(T Numerator, T Denominator, T Fallback = T(0))
    {
        return IsNearlyZero(static_cast<float>(Denominator)) ? Fallback : Numerator / Denominator;
    }
};