#pragma once
#include "Types.h"

class UEngine;

class FEngineLoop
{
private:
	UEngine* Engine = nullptr;
	HWND WindowHandle = nullptr;

	float LastFrameTime = 0.0f;
	float DeltaTime = 0.0f;

	bool bIsEditor = true;
	bool bIsInitialized = false;

public:
	FEngineLoop();
	~FEngineLoop();

	bool PreInit(HINSTANCE hInstance, HWND hWnd, bool bEditor = true);
	bool Init();
	void Tick();
	void Exit();

	UEngine* GetEngine() const { return Engine; }
	bool IsInitialized() const { return bIsInitialized; }
	bool IsEditor() const { return bIsEditor; }

	float GetDeltaTime() const { return DeltaTime; }

private:
	void CalculateDeltaTime();
	float GetCurrentTimeSeconds() const;
};
