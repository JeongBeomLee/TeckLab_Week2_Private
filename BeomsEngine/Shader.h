#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "ShaderParameterMap.h"
#include "String.h"

// 전방 선언
class FBufferManager;

// 셰이더 베이스 클래스 (언리얼의 FShader 역할)
class FShader
{
protected:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;

	FShaderParameterMap ParameterMap;

	FString ShaderPath;
	FString VSEntryPoint;
	FString PSEntryPoint;

	bool bInitialized;

public:
	FShader();
	virtual ~FShader();

	// 셰이더 초기화 (서브클래스에서 호출)
	virtual bool Initialize(
		ID3D11Device* Device,
		const FString& InShaderPath,
		const FString& InVSEntry,
		const FString& InPSEntry
	);

	// 셰이더 바인딩 (렌더링 직전 호출)
	void Bind(ID3D11DeviceContext* Context);

	// 파라미터 설정 (드로우 직전 호출)
	template<typename T>
	void SetConstantBuffer(
		ID3D11DeviceContext* Context,
		const FString& BufferName,
		const T& Data
	);

	// 리소스 해제
	void ReleaseResources();

	// Getters
	bool IsInitialized() const { return bInitialized; }
	const FShaderParameterMap& GetParameterMap() const { return ParameterMap; }
	FString GetShaderPath() const { return ShaderPath; }

protected:
	// 서브클래스에서 구현 (Input Layout 생성)
	virtual void CreateInputLayout(ID3D11Device* Device, ID3DBlob* VSBlob) = 0;

	// 서브클래스에서 구현 (파라미터 맵 설정)
	virtual void SetupParameterMap() = 0;

	// 셰이더 컴파일 헬퍼
	bool CompileShaderFromFile(
		ID3D11Device* Device,
		const wchar_t* FileName,
		const char* EntryPoint,
		const char* ShaderModel,
		ID3DBlob** OutBlob
	);
};

// 전방 선언을 위한 include (템플릿이므로 헤더에 필요)
#include "BufferManager.h"

// 템플릿 메서드 구현
template<typename T>
void FShader::SetConstantBuffer(
	ID3D11DeviceContext* Context,
	const FString& BufferName,
	const T& Data)
{
	if (!Context || !bInitialized)
	{
		return;
	}

	// 파라미터 맵에서 바인딩 정보 조회
	FShaderParameterBinding Binding;
	if (!ParameterMap.FindParameter(BufferName, Binding))
	{
		return;
	}

	// FBufferManager에서 버퍼 가져오기 (없으면 생성)
	TBuffer<T>* Buffer = FBufferManager::Get().GetOrCreateConstantBuffer<T>(BufferName);
	if (!Buffer)
	{
		return;
	}

	// 데이터 업데이트
	Buffer->Update(Context, Data);

	// 셰이더에 바인딩
	Buffer->BindAsConstantBuffer(Context, Binding.RegisterSlot, Binding.ShaderStage);
}
