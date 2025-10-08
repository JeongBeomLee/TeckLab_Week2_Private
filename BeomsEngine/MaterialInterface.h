#pragma once
#include "Object.h"
#include "Vector.h"
#include "Name.h"
#include "String.h"

// 전방 선언
class FMaterialResource;
class UMaterial;

// Blend Mode Enum
enum class EBlendMode : uint8
{
	BLEND_Opaque,
	BLEND_Masked,
	BLEND_Translucent,
	BLEND_Additive,
};

// Shading Model Enum
enum class EShadingModel : uint8
{
	MSM_DefaultLit,
	MSM_Unlit,
};

// Material 추상 베이스 클래스 (UObject 상속)
class UMaterialInterface : public UObject
{
	UCLASS()
	GENERATED_ABSTRACT_BODY(UMaterialInterface, UObject)

public:
	UMaterialInterface();
	virtual ~UMaterialInterface();

	// 실제 렌더링 리소스 가져오기 (순수 가상)
	virtual FMaterialResource* GetMaterialResource() = 0;
	virtual const FMaterialResource* GetMaterialResource() const = 0;

	// Material Instance 체인 탐색 (순수 가상)
	virtual UMaterial* GetBaseMaterial() = 0;

	// Material 이름 가져오기 (순수 가상)
	virtual FString GetMaterialName() const = 0;

	// 파라미터 접근 (서브클래스에서 오버라이드 가능)
	virtual bool GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const;
	virtual bool GetScalarParameterValue(const FName& ParamName, float& OutValue) const;

	// Material 속성 조회
	virtual EBlendMode GetBlendMode() const { return EBlendMode::BLEND_Opaque; }
	virtual EShadingModel GetShadingModel() const { return EShadingModel::MSM_DefaultLit; }
	virtual bool IsTwoSided() const { return false; }
	virtual bool IsMasked() const { return GetBlendMode() == EBlendMode::BLEND_Masked; }
	virtual bool IsTranslucent() const
	{
		EBlendMode Mode = GetBlendMode();
		return Mode == EBlendMode::BLEND_Translucent || Mode == EBlendMode::BLEND_Additive;
	}
};
