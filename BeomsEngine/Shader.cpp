#include "pch.h"
#include "Shader.h"

FShader::FShader()
	: VertexShader(nullptr)
	, PixelShader(nullptr)
	, InputLayout(nullptr)
	, bInitialized(false)
{
}

FShader::~FShader()
{
	ReleaseResources();
}

bool FShader::Initialize(
	ID3D11Device* Device,
	const FString& InShaderPath,
	const FString& InVSEntry,
	const FString& InPSEntry)
{
	if (!Device)
	{
		return false;
	}

	if (bInitialized)
	{
		ReleaseResources();
	}

	ShaderPath = InShaderPath;
	VSEntryPoint = InVSEntry;
	PSEntryPoint = InPSEntry;

	// Vertex Shader 컴파일
	ID3DBlob* VSBlob = nullptr;
	std::wstring WPath(ShaderPath.begin(), ShaderPath.end());

	if (!CompileShaderFromFile(Device, WPath.c_str(), VSEntryPoint.c_str(), "vs_5_0", &VSBlob))
	{
		return false;
	}

	// Vertex Shader 생성
	HRESULT hr = Device->CreateVertexShader(
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		nullptr,
		&VertexShader
	);

	if (FAILED(hr))
	{
		VSBlob->Release();
		return false;
	}

	// Pixel Shader 컴파일
	ID3DBlob* PSBlob = nullptr;
	if (!CompileShaderFromFile(Device, WPath.c_str(), PSEntryPoint.c_str(), "ps_5_0", &PSBlob))
	{
		VSBlob->Release();
		return false;
	}

	// Pixel Shader 생성
	hr = Device->CreatePixelShader(
		PSBlob->GetBufferPointer(),
		PSBlob->GetBufferSize(),
		nullptr,
		&PixelShader
	);

	if (FAILED(hr))
	{
		VSBlob->Release();
		PSBlob->Release();
		return false;
	}

	// Input Layout 생성 (서브클래스 구현)
	CreateInputLayout(Device, VSBlob);

	// Blob 해제
	VSBlob->Release();
	PSBlob->Release();

	// 파라미터 맵 설정 (서브클래스 구현)
	SetupParameterMap();

	bInitialized = true;
	return true;
}

void FShader::Bind(ID3D11DeviceContext* Context)
{
	if (!Context || !bInitialized)
	{
		return;
	}

	// Vertex Shader 바인딩
	Context->VSSetShader(VertexShader, nullptr, 0);

	// Pixel Shader 바인딩
	Context->PSSetShader(PixelShader, nullptr, 0);

	// Input Layout 바인딩
	Context->IASetInputLayout(InputLayout);
}

void FShader::ReleaseResources()
{
	if (VertexShader)
	{
		VertexShader->Release();
		VertexShader = nullptr;
	}

	if (PixelShader)
	{
		PixelShader->Release();
		PixelShader = nullptr;
	}

	if (InputLayout)
	{
		InputLayout->Release();
		InputLayout = nullptr;
	}

	ParameterMap.Clear();
	bInitialized = false;
}

bool FShader::CompileShaderFromFile(
	ID3D11Device* Device,
	const wchar_t* FileName,
	const char* EntryPoint,
	const char* ShaderModel,
	ID3DBlob** OutBlob)
{
	if (!FileName || !EntryPoint || !ShaderModel || !OutBlob)
	{
		return false;
	}

	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	ShaderFlags |= D3DCOMPILE_DEBUG;
	ShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* ErrorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		FileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		EntryPoint,
		ShaderModel,
		ShaderFlags,
		0,
		OutBlob,
		&ErrorBlob
	);

	if (FAILED(hr))
	{
		if (ErrorBlob)
		{
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			ErrorBlob->Release();
		}
		return false;
	}

	if (ErrorBlob)
	{
		ErrorBlob->Release();
	}

	return true;
}
