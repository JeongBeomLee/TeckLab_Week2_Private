#include "pch.h"
#include "MaterialInterface.h"

IMPLEMENT_CLASS(UMaterialInterface, UObject)

UMaterialInterface::UMaterialInterface()
{
}

UMaterialInterface::~UMaterialInterface()
{
}

bool UMaterialInterface::GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const
{
	// 기본 구현 (서브클래스에서 오버라이드)
	return false;
}

bool UMaterialInterface::GetScalarParameterValue(const FName& ParamName, float& OutValue) const
{
	// 기본 구현 (서브클래스에서 오버라이드)
	return false;
}
