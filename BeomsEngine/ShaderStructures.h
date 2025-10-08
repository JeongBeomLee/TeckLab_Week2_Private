#pragma once
#include "Math.h"

// 모든 Constant Buffer 구조체 정의
// HLSL cbuffer와 메모리 레이아웃이 일치해야 함

// MVP 변환 행렬 (register(b0))
struct FMVPConstantBuffer
{
	FMatrix World;
	FMatrix View;
	FMatrix Projection;
};
