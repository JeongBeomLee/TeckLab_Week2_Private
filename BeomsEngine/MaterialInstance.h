#pragma once
#include "MaterialInterface.h"
#include "Containers.h"

// 전방 선언
class UMaterial;

// Material Instance 클래스 (파라미터 오버라이드)
class UMaterialInstance : public UMaterialInterface
{
	UCLASS()
	GENERATED_BODY(UMaterialInstance, UMaterialInterface)

private:
	// 부모 Material 참조
	UMaterial* ParentMaterial;

	// 오버라이드된 파라미터
	TMap<FName, FVector> VectorParameterOverrides;
	TMap<FName, float> ScalarParameterOverrides;

public:
	UMaterialInstance();
	virtual ~UMaterialInstance();

	// MaterialInterface 구현
	virtual FMaterialResource* GetMaterialResource() override;
	virtual const FMaterialResource* GetMaterialResource() const override;
	virtual UMaterial* GetBaseMaterial() override { return ParentMaterial; }
	virtual FString GetMaterialName() const override;

	// Parent 설정
	void SetParentMaterial(UMaterial* Parent);
	UMaterial* GetParent() const { return ParentMaterial; }

	// 파라미터 오버라이드
	void SetVectorParameterOverride(const FName& ParamName, const FVector& Value);
	void SetScalarParameterOverride(const FName& ParamName, float Value);

	// 파라미터 조회 (오버라이드 → 부모 순)
	virtual bool GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const override;
	virtual bool GetScalarParameterValue(const FName& ParamName, float& OutValue) const override;
};
