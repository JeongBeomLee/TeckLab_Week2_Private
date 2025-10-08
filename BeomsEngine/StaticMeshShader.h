#pragma once
#include "Shader.h"

// StaticMesh 렌더링을 위한 셰이더 클래스
class FStaticMeshShader : public FShader
{
public:
	FStaticMeshShader();
	virtual ~FStaticMeshShader();

protected:
	// FShader 순수 가상 함수 구현
	virtual void CreateInputLayout(ID3D11Device* Device, ID3DBlob* VSBlob) override;
	virtual void SetupParameterMap() override;
};
