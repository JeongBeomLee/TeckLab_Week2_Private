#include "pch.h"
#include "ShaderParameterMap.h"

FShaderParameterMap::FShaderParameterMap()
{
}

void FShaderParameterMap::AddParameter(
	const FString& Name,
	uint32 RegisterSlot,
	uint32 BufferSize,
	EShaderStage Stage)
{
	// 중복 확인
	if (NameToIndexMap.find(Name) != NameToIndexMap.end())
	{
		return;
	}

	// 새 바인딩 추가
	FShaderParameterBinding Binding(Name, RegisterSlot, BufferSize, Stage);
	Bindings.push_back(Binding);

	// 인덱스 매핑
	uint32 Index = static_cast<uint32>(Bindings.size() - 1);
	NameToIndexMap[Name] = Index;
}

bool FShaderParameterMap::FindParameter(const FString& Name, FShaderParameterBinding& OutBinding) const
{
	auto it = NameToIndexMap.find(Name);
	if (it == NameToIndexMap.end())
	{
		return false;
	}

	uint32 Index = it->second;
	if (Index >= Bindings.size())
	{
		return false;
	}

	OutBinding = Bindings[Index];
	return true;
}

const FShaderParameterBinding* FShaderParameterMap::GetParameterByName(const FString& Name) const
{
	auto it = NameToIndexMap.find(Name);
	if (it == NameToIndexMap.end())
	{
		return nullptr;
	}

	uint32 Index = it->second;
	if (Index >= Bindings.size())
	{
		return nullptr;
	}

	return &Bindings[Index];
}

void FShaderParameterMap::Clear()
{
	Bindings.clear();
	NameToIndexMap.clear();
}
