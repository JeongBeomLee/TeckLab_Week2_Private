#include "pch.h"
#include "EngineLoop.h"
#include "Engine.h"
#include "EditorEngine.h"
#include "ObjectInitializer.h"

UEngine* GEngine = nullptr;

FEngineLoop::FEngineLoop()
	: Engine(nullptr)
	, WindowHandle(nullptr)
	, LastFrameTime(0.0f)
	, DeltaTime(0.0f)
	, bIsEditor(true)
	, bIsInitialized(false)
{
}

FEngineLoop::~FEngineLoop()
{
	Exit();
}

bool FEngineLoop::PreInit(HINSTANCE hInstance, HWND hWnd, bool bEditor)
{
	WindowHandle = hWnd;
	bIsEditor = bEditor;

	if (bIsEditor)
	{
		Engine = NewObject<UEditorEngine>();
	}
	else
	{
		Engine = NewObject<UEngine>();
	}

	if (!Engine)
	{
		return false;
	}

	GEngine = Engine;

	return true;
}

bool FEngineLoop::Init()
{
	if (!Engine)
	{
		return false;
	}

	RECT ClientRect;
	GetClientRect(WindowHandle, &ClientRect);
	uint32 Width = ClientRect.right - ClientRect.left;
	uint32 Height = ClientRect.bottom - ClientRect.top;

	if (!Engine->Init(WindowHandle, Width, Height))
	{
		return false;
	}

	LastFrameTime = GetCurrentTimeSeconds();
	bIsInitialized = true;

	return true;
}

void FEngineLoop::Tick()
{
	if (!bIsInitialized || !Engine)
	{
		return;
	}

	CalculateDeltaTime();

	Engine->Tick(DeltaTime);
}

void FEngineLoop::Exit()
{
	if (!bIsInitialized)
	{
		return;
	}

	if (Engine)
	{
		Engine->Shutdown();
		Engine = nullptr;
	}

	GEngine = nullptr;
	bIsInitialized = false;
}

void FEngineLoop::CalculateDeltaTime()
{
	float CurrentTime = GetCurrentTimeSeconds();
	DeltaTime = CurrentTime - LastFrameTime;
	LastFrameTime = CurrentTime;

	if (DeltaTime > 0.1f)
	{
		DeltaTime = 0.1f;
	}
}

float FEngineLoop::GetCurrentTimeSeconds() const
{
	static LARGE_INTEGER Frequency;
	static LARGE_INTEGER StartTime;
	static bool bInitialized = false;

	if (!bInitialized)
	{
		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&StartTime);
		bInitialized = true;
	}

	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);

	return static_cast<float>(CurrentTime.QuadPart - StartTime.QuadPart) / static_cast<float>(Frequency.QuadPart);
}
