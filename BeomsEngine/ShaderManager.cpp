#include "pch.h"
#include "ShaderManager.h"

FShaderManager* FShaderManager::Instance = nullptr;

FShaderManager::FShaderManager()
	: Device(nullptr)
{
}

FShaderManager::~FShaderManager()
{
	ClearCache();
}

FShaderManager& FShaderManager::Get()
{
	if (!Instance)
	{
		Instance = new FShaderManager();
	}
	return *Instance;
}

void FShaderManager::Initialize(ID3D11Device* InDevice)
{
	if (!Instance)
	{
		Instance = new FShaderManager();
	}
	Instance->Device = InDevice;
}

void FShaderManager::Shutdown()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}

void FShaderManager::RegisterShader(const FString& ShaderName, FShader* Shader)
{
	if (!Shader)
	{
		return;
	}

	// 기존 셰이더가 있으면 제거
	auto it = ShaderCache.find(ShaderName);
	if (it != ShaderCache.end())
	{
		delete it->second;
		ShaderCache.erase(it);
	}

	// 새 셰이더 등록
	ShaderCache[ShaderName] = Shader;
}

FShader* FShaderManager::FindShader(const FString& ShaderName)
{
	auto it = ShaderCache.find(ShaderName);
	if (it != ShaderCache.end())
	{
		return it->second;
	}
	return nullptr;
}

void FShaderManager::ClearCache()
{
	for (auto& Pair : ShaderCache)
	{
		if (Pair.second)
		{
			delete Pair.second;
		}
	}
	ShaderCache.clear();
}
