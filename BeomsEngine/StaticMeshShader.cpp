#include "pch.h"
#include "StaticMeshShader.h"
#include "ShaderStructures.h"

FStaticMeshShader::FStaticMeshShader()
{
}

FStaticMeshShader::~FStaticMeshShader()
{
}

void FStaticMeshShader::CreateInputLayout(ID3D11Device* Device, ID3DBlob* VSBlob)
{
	if (!Device || !VSBlob)
	{
		return;
	}

	// FVertex 구조에 맞는 Input Layout 정의
	// HLSL VS_INPUT: POSITION, NORMAL, TEXCOORD0
	D3D11_INPUT_ELEMENT_DESC InputElements[] =
	{
		// Semantic, Index, Format, InputSlot, AlignedByteOffset, InputSlotClass, InstanceDataStepRate
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },   // FVector Position (offset 0)
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  // FVector Normal (offset 12)
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }   // FVector2 UV (offset 48, Tangent+Binormal 스킵)
	};

	UINT NumElements = ARRAYSIZE(InputElements);

	// Input Layout 생성
	HRESULT hr = Device->CreateInputLayout(
		InputElements,
		NumElements,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&InputLayout
	);

	if (FAILED(hr))
	{
		InputLayout = nullptr;
	}
}

void FStaticMeshShader::SetupParameterMap()
{
	// MVPBuffer를 register(b0), Vertex Shader에 등록
	ParameterMap.AddParameter(
		"MVPBuffer",
		0,  // register(b0)
		sizeof(FMVPConstantBuffer),
		EShaderStage::Vertex
	);
}
