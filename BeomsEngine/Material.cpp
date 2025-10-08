#include "pch.h"
#include "Material.h"
#include "MaterialResource.h"

IMPLEMENT_CLASS(UMaterial, UMaterialInterface)

UMaterial::UMaterial()
	: MaterialResource(nullptr)
{
}

UMaterial::~UMaterial()
{
	if (MaterialResource)
	{
		delete MaterialResource;
		MaterialResource = nullptr;
	}
}

void UMaterial::SetShaderPath(const FString& Path)
{
	ShaderPath = Path;
}

bool UMaterial::CompileMaterial(ID3D11Device* Device)
{
	if (!Device || ShaderPath.empty())
	{
		return false;
	}

	// MaterialResource 생성 (없으면)
	if (!MaterialResource)
	{
		MaterialResource = new FMaterialResource(this);
	}

	// Material 컴파일 (다음 단계에서 FMaterialResource 구현 후 작동)
	return MaterialResource->CompileMaterial(Device, ShaderPath);
}

void UMaterial::SetVectorParameter(const FName& ParamName, const FVector& Value)
{
	VectorParameters[ParamName] = Value;
}

void UMaterial::SetScalarParameter(const FName& ParamName, float Value)
{
	ScalarParameters[ParamName] = Value;
}

bool UMaterial::GetVectorParameterValue(const FName& ParamName, FVector& OutValue) const
{
	auto it = VectorParameters.find(ParamName);
	if (it != VectorParameters.end())
	{
		OutValue = it->second;
		return true;
	}
	return false;
}

bool UMaterial::GetScalarParameterValue(const FName& ParamName, float& OutValue) const
{
	auto it = ScalarParameters.find(ParamName);
	if (it != ScalarParameters.end())
	{
		OutValue = it->second;
		return true;
	}
	return false;
}
