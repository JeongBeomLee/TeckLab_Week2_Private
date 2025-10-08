#include "pch.h"
#include "MaterialResource.h"
#include "Shader.h"
#include "StaticMeshShader.h"
#include "ShaderManager.h"
#include "Material.h"

FMaterialResource::FMaterialResource(UMaterial* Owner)
	: Shader(nullptr)
	, OwnerMaterial(Owner)
	, bInitialized(false)
{
}

FMaterialResource::~FMaterialResource()
{
	ReleaseResources();
}

bool FMaterialResource::CompileMaterial(ID3D11Device* Device, const FString& ShaderPath)
{
	if (!Device || ShaderPath.empty())
	{
		return false;
	}

	// FShaderManager를 통해 셰이더 가져오기 (캐싱됨)
	// 현재는 StaticMeshShader만 지원
	FString ShaderName = ShaderPath;  // 셰이더 이름으로 경로 사용

	Shader = FShaderManager::Get().GetOrCreateShader<FStaticMeshShader>(
		ShaderName,
		ShaderPath,
		"VS",  // Vertex Shader 진입점
		"PS"   // Pixel Shader 진입점
	);

	if (!Shader)
	{
		bInitialized = false;
		return false;
	}

	bInitialized = true;
	return true;
}

void FMaterialResource::BindParameters(ID3D11DeviceContext* Context)
{
	// Material 파라미터를 Shader에 전달
	// 현재는 MVP만 사용하므로 비워둠
	// 향후 Material 파라미터가 추가되면 여기서 SetConstantBuffer 호출

	// 예시 (향후 확장):
	// if (OwnerMaterial)
	// {
	//     FVector BaseColor;
	//     if (OwnerMaterial->GetVectorParameterValue("BaseColor", BaseColor))
	//     {
	//         // Material Parameter Buffer 업데이트
	//     }
	// }
}

void FMaterialResource::ReleaseResources()
{
	// Shader는 FShaderManager가 소유하므로 삭제하지 않음
	Shader = nullptr;
	bInitialized = false;
}
