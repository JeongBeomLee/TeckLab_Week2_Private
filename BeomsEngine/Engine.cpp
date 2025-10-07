#include "pch.h"
#include "Engine.h"
#include "D3D11GraphicsDevice.h"
#include "Renderer.h"
#include "World.h"

IMPLEMENT_CLASS(UEngine, UObject)

UEngine::UEngine()
	: GraphicsDevice(nullptr)
	, Renderer(nullptr)
	, World(nullptr)
	, bIsInitialized(false)
	, bIsRunning(false)
{
}

UEngine::~UEngine()
{
	Shutdown();
}

bool UEngine::Init(HWND WindowHandle, uint32 WindowWidth, uint32 WindowHeight)
{
	if (bIsInitialized)
	{
		return true;
	}

	if (!InitializeGraphicsDevice(WindowHandle, WindowWidth, WindowHeight))
	{
		return false;
	}

	if (!InitializeRenderer())
	{
		return false;
	}

	if (!InitializeWorld())
	{
		return false;
	}

	bIsInitialized = true;
	bIsRunning = true;

	return true;
}

void UEngine::Tick(float DeltaTime)
{
	if (!bIsInitialized || !bIsRunning)
	{
		return;
	}

	TickWorld(DeltaTime);
	TickRendering(DeltaTime);

	GUObjectArray.PerformGarbageCollector();
}

void UEngine::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}

	bIsRunning = false;

	ShutdownWorld();
	ShutdownRenderer();
	ShutdownGraphicsDevice();

	FNamePool::DistroyInstance();

	bIsInitialized = false;
}

bool UEngine::InitializeGraphicsDevice(HWND WindowHandle, uint32 WindowWidth, uint32 WindowHeight)
{
	GraphicsDevice = new FD3D11GraphicsDevice();
	if (!GraphicsDevice)
	{
		return false;
	}

	FD3D11GraphicsDevice::FDeviceDesc DeviceDesc;
	DeviceDesc.WindowHandle = WindowHandle;
	DeviceDesc.WindowWidth = WindowWidth;
	DeviceDesc.WindowHeight = WindowHeight;
	DeviceDesc.bFullscreen = false;
	DeviceDesc.bVSync = true;

	if (!GraphicsDevice->Init(DeviceDesc))
	{
		delete GraphicsDevice;
		GraphicsDevice = nullptr;
		return false;
	}

	return true;
}

bool UEngine::InitializeRenderer()
{
	if (!GraphicsDevice || !GraphicsDevice->IsInitialized())
	{
		return false;
	}

	Renderer = NewObject<URenderer>();
	if (!Renderer)
	{
		return false;
	}

	Renderer->InitializeRenderer(GraphicsDevice->GetContext(), GraphicsDevice);
	return Renderer->IsInitialized();
}

bool UEngine::InitializeWorld()
{
	World = UWorld::CreateWorld(FName("EditorWorld"));
	if (!World)
	{
		return false;
	}

	World->BeginPlay();
	return true;
}

void UEngine::TickWorld(float DeltaTime)
{
	if (World)
	{
		World->Tick(DeltaTime);
	}
}

void UEngine::TickRendering(float DeltaTime)
{
	if (!GraphicsDevice || !Renderer)
	{
		return;
	}

	GraphicsDevice->ClearRenderTargets();
	Renderer->RenderScene();
	GraphicsDevice->SwapBuffer();
}

void UEngine::ShutdownWorld()
{
	if (World)
	{
		UWorld::DestroyWorld(World);
		World = nullptr;
	}
}

void UEngine::ShutdownRenderer()
{
	if (Renderer)
	{
		Renderer->Shutdown();
		Renderer = nullptr;
	}
}

void UEngine::ShutdownGraphicsDevice()
{
	if (GraphicsDevice)
	{
		GraphicsDevice->Shutdown();
		delete GraphicsDevice;
		GraphicsDevice = nullptr;
	}
}
