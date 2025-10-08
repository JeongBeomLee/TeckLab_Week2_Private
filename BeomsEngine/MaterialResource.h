#pragma once
#include <d3d11.h>

// 전방 선언
class FShader;
class UMaterial;

// Material GPU 렌더링 리소스 (FShader 소유)
class FMaterialResource
{
private:
	FShader* Shader;              // 컴파일된 셰이더
	UMaterial* OwnerMaterial;     // 소유자 Material
	bool bInitialized;

public:
	FMaterialResource(UMaterial* Owner);
	~FMaterialResource();

	// Material 컴파일 (셰이더 생성)
	bool CompileMaterial(ID3D11Device* Device, const FString& ShaderPath);

	// 렌더링 시 사용
	FShader* GetShader() const { return Shader; }
	UMaterial* GetOwner() const { return OwnerMaterial; }

	// 파라미터 전달 (Material → Shader)
	void BindParameters(ID3D11DeviceContext* Context);

	// 리소스 해제
	void ReleaseResources();

	bool IsInitialized() const { return bInitialized; }
};
