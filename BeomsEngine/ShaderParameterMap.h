#pragma once
#include "Types.h"
#include "Containers.h"
#include "String.h"
#include "TBuffer.h"

// 셰이더 파라미터 바인딩 정보
struct FShaderParameterBinding
{
	FString ParameterName;     // HLSL 변수명 (예: "MVPBuffer")
	uint32 RegisterSlot;       // 레지스터 슬롯 (b0, b1, b2...)
	uint32 BufferSize;         // 바이트 크기
	EShaderStage ShaderStage;  // 셰이더 스테이지 (VS, PS, ...)

	FShaderParameterBinding()
		: RegisterSlot(0)
		, BufferSize(0)
		, ShaderStage(EShaderStage::Vertex)
	{
	}

	FShaderParameterBinding(const FString& InName, uint32 InSlot, uint32 InSize, EShaderStage InStage)
		: ParameterName(InName)
		, RegisterSlot(InSlot)
		, BufferSize(InSize)
		, ShaderStage(InStage)
	{
	}
};

// HLSL 변수와 GPU 레지스터 슬롯 매핑 정보 관리
class FShaderParameterMap
{
private:
	TArray<FShaderParameterBinding> Bindings;
	TMap<FString, uint32> NameToIndexMap;

public:
	FShaderParameterMap();

	// 파라미터 등록
	void AddParameter(
		const FString& Name,
		uint32 RegisterSlot,
		uint32 BufferSize,
		EShaderStage Stage
	);

	// 파라미터 조회
	bool FindParameter(const FString& Name, FShaderParameterBinding& OutBinding) const;
	const FShaderParameterBinding* GetParameterByName(const FString& Name) const;

	// 전체 바인딩 정보 접근
	const TArray<FShaderParameterBinding>& GetBindings() const { return Bindings; }

	// 초기화
	void Clear();
};
