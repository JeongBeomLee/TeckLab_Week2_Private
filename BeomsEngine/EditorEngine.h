#pragma once
#include "Engine.h"
#include "SharedPointer.h"

class SLevelViewport;
class FEditorViewportClient;

class UEditorEngine : public UEngine
{
	UCLASS()
	GENERATED_BODY(UEditorEngine, UEngine)

private:
	TSharedPtr<SLevelViewport> MainViewport;
	FEditorViewportClient* ViewportClient = nullptr;

	bool bEditorInitialized = false;

public:
	UEditorEngine();
	virtual ~UEditorEngine();

	virtual FString GetClassName() const override { return "UEditorEngine"; }

	TSharedPtr<SLevelViewport> GetMainViewport() const { return MainViewport; }
	FEditorViewportClient* GetViewportClient() const { return ViewportClient; }

	bool IsEditorInitialized() const { return bEditorInitialized; }

	virtual bool Init(HWND WindowHandle, uint32 WindowWidth = 1920, uint32 WindowHeight = 1080) override;
	virtual void Tick(float DeltaTime) override;
	virtual void Shutdown() override;

private:
	bool CreateMainViewport();
	bool CreateViewportClient();

	void UpdateEditorViewport(float DeltaTime);
	void RenderEditorViewport();
};
