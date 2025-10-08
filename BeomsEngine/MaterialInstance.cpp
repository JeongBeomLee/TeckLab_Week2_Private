#include "pch.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialResource.h"

IMPLEMENT_CLASS(UMaterialInstance, UMaterialInterface)

UMaterialInstance::UMaterialInstance()
	: ParentMaterial(nullptr)
{
}

UMaterialInstance::~UMaterialInstance()
{
	// Parent Material은 소유하지 않으므로 삭제하지 않음
}

FMaterialResource* UMaterialInstance::GetMaterialResource()
{
	// Parent Material의 Resource 반환
	if (ParentMaterial)
	{
		return ParentMaterial->GetMaterialResource();
	}
	return nullptr;
}

const FMaterialResource* UMaterialInstance::GetMaterialResource() const
{
	// Parent Material의 Resource 반환
	if (ParentMaterial)
	{
		return ParentMaterial->GetMaterialResource();
	}
	return nullptr;
}

FString UMaterialInstance::GetMaterialName() const
{
	if (ParentMaterial)
	{
		return ParentMaterial->GetMaterialName() + "_Inst";
	}
	return "MaterialInstance";
}

void UMaterialInstance::SetParentMaterial(UMaterial* Parent)
{
	ParentMaterial = Parent;
}

void UMaterialInstance::SetVectorParameterOverride(const FName& ParamName, const FVector& Value)
{
	VectorParameterOverrides[ParamName] = Value;
}

void UMaterialInstance::SetScalarParameterOverride(const FName& ParamName, float Value)
{
	ScalarParameterOverrides[ParamName] = Value;
}

bool UMaterialInstance::GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const
{
	// 1. 먼저 오버라이드에서 찾기
	auto it = VectorParameterOverrides.find(ParamName);
	if (it != VectorParameterOverrides.end())
	{
		OutValue = it->second;
		return true;
	}

	// 2. 오버라이드에 없으면 부모에서 찾기
	if (ParentMaterial)
	{
		return ParentMaterial->GetVectorParameterValue(ParamName, OutValue);
	}

	return false;
}

bool UMaterialInstance::GetScalarParameterValue(const FName& ParamName, float& OutValue) const
{
	// 1. 먼저 오버라이드에서 찾기
	auto it = ScalarParameterOverrides.find(ParamName);
	if (it != ScalarParameterOverrides.end())
	{
		OutValue = it->second;
		return true;
	}

	// 2. 오버라이드에 없으면 부모에서 찾기
	if (ParentMaterial)
	{
		return ParentMaterial->GetScalarParameterValue(ParamName, OutValue);
	}

	return false;
}
