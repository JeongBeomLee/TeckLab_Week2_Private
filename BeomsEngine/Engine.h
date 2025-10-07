#pragma once
#include "Object.h"
#include "Types.h"

class FD3D11GraphicsDevice;
class URenderer;
class UWorld;

class UEngine : public UObject
{
	UCLASS()
	GENERATED_BODY(UEngine, UObject)

protected:
	FD3D11GraphicsDevice* GraphicsDevice = nullptr;
	URenderer* Renderer = nullptr;
	UWorld* World = nullptr;

	bool bIsInitialized = false;
	bool bIsRunning = false;

public:
	UEngine();
	virtual ~UEngine();

	virtual FString GetClassName() const override { return "UEngine"; }

	virtual bool Init(HWND WindowHandle, uint32 WindowWidth = 1920, uint32 WindowHeight = 1080);
	virtual void Tick(float DeltaTime);
	virtual void Shutdown();

	bool IsInitialized() const { return bIsInitialized; }
	bool IsRunning() const { return bIsRunning; }

	FD3D11GraphicsDevice* GetGraphicsDevice() const { return GraphicsDevice; }
	URenderer* GetRenderer() const { return Renderer; }
	UWorld* GetWorld() const { return World; }

protected:
	virtual bool InitializeGraphicsDevice(HWND WindowHandle, uint32 WindowWidth, uint32 WindowHeight);
	virtual bool InitializeRenderer();
	virtual bool InitializeWorld();

	virtual void TickWorld(float DeltaTime);
	virtual void TickRendering(float DeltaTime);

	virtual void ShutdownWorld();
	virtual void ShutdownRenderer();
	virtual void ShutdownGraphicsDevice();
};
