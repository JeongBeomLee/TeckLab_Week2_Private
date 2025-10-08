#pragma once
#include "MaterialInterface.h"
#include "Containers.h"

// 전방 선언
class FMaterialResource;

// Material 클래스 (셰이더 소스 소유)
class UMaterial : public UMaterialInterface
{
	UCLASS()
	GENERATED_BODY(UMaterial, UMaterialInterface)

private:
	// GPU 렌더링 리소스
	FMaterialResource* MaterialResource;

	// Material 속성
	FString ShaderPath;
	FString MaterialName;

	// 기본 파라미터 (Instance가 오버라이드 가능)
	TMap<FName, FVector> VectorParameters;
	TMap<FName, float> ScalarParameters;

public:
	UMaterial();
	virtual ~UMaterial();

	// MaterialInterface 구현
	virtual FMaterialResource* GetMaterialResource() override { return MaterialResource; }
	virtual const FMaterialResource* GetMaterialResource() const override { return MaterialResource; }
	virtual UMaterial* GetBaseMaterial() override { return this; }
	virtual FString GetMaterialName() const override { return MaterialName; }

	// Material 설정
	void SetMaterialName(const FString& Name) { MaterialName = Name; }
	void SetShaderPath(const FString& Path);
	FString GetShaderPath() const { return ShaderPath; }

	// Material 컴파일 (셰이더 생성 및 리소스 초기화)
	bool CompileMaterial(ID3D11Device* Device);

	// 파라미터 설정
	void SetVectorParameter(const FName& ParamName, const FVector& Value);
	void SetScalarParameter(const FName& ParamName, float Value);

	// 파라미터 조회 (오버라이드)
	virtual bool GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const override;
	virtual bool GetScalarParameterValue(const FName& ParamName, float& OutValue) const override;

	// 파라미터 맵 접근
	const TMap<FName, FVector>& GetVectorParameters() const { return VectorParameters; }
	const TMap<FName, float>& GetScalarParameters() const { return ScalarParameters; }
};
