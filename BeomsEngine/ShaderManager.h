#pragma once
#include <d3d11.h>
#include "Containers.h"
#include "String.h"
#include "Shader.h"

// FShader 객체를 캐싱하는 싱글톤 클래스 (언리얼의 FGlobalShaderMap 역할)
class FShaderManager
{
private:
	static FShaderManager* Instance;

	ID3D11Device* Device;

	// FShader 캐싱 (타입명/경로 기반)
	TMap<FString, FShader*> ShaderCache;

	// 생성자 private (싱글톤)
	FShaderManager();
	~FShaderManager();

	// 복사 방지
	FShaderManager(const FShaderManager&) = delete;
	FShaderManager& operator=(const FShaderManager&) = delete;

public:
	// 싱글톤 접근
	static FShaderManager& Get();
	static void Initialize(ID3D11Device* InDevice);
	static void Shutdown();

	// 셰이더 등록 및 조회
	void RegisterShader(const FString& ShaderName, FShader* Shader);
	FShader* FindShader(const FString& ShaderName);

	// 셰이더 가져오기 (없으면 생성)
	template<typename TShaderClass>
	TShaderClass* GetOrCreateShader(
		const FString& ShaderName,
		const FString& ShaderPath,
		const FString& VSEntry,
		const FString& PSEntry
	);

	// Device 접근
	ID3D11Device* GetDevice() const { return Device; }

	// 캐시 정리
	void ClearCache();
};

// 템플릿 메서드 구현
template<typename TShaderClass>
TShaderClass* FShaderManager::GetOrCreateShader(
	const FString& ShaderName,
	const FString& ShaderPath,
	const FString& VSEntry,
	const FString& PSEntry)
{
	static_assert(std::is_base_of<FShader, TShaderClass>::value, "TShaderClass must derive from FShader");

	// 캐시 확인
	FShader* CachedShader = FindShader(ShaderName);
	if (CachedShader)
	{
		return static_cast<TShaderClass*>(CachedShader);
	}

	// 새로 생성
	TShaderClass* NewShader = new TShaderClass();
	if (!NewShader->Initialize(Device, ShaderPath, VSEntry, PSEntry))
	{
		delete NewShader;
		return nullptr;
	}

	// 캐싱
	RegisterShader(ShaderName, NewShader);

	return NewShader;
}
