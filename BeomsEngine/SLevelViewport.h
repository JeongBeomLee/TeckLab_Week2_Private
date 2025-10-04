#pragma once
#include "SEditorViewport.h"
#include "SPanel.h"
#include "Delegate.h"

class ULevel;
class UWorld;

enum class ELevelViewportType
{
    Perspective,
    Orthographic_Top,
    Orthographic_Side,
    Orthographic_Front
};

class SLevelViewportToolbar : public SHorizontalBox
{
public:
    SLevelViewportToolbar();
    virtual ~SLevelViewportToolbar() = default;

private:
    TSharedPtr<SButton> ViewModeButton;
    TSharedPtr<SButton> CameraModeButton;
    TSharedPtr<SButton> RealTimeButton;
    TSharedPtr<STextBlock> ViewportStatsText;

public:
    void Initialize();
    void SetViewportType(ELevelViewportType InType);
    void SetRealTimeEnabled(bool bEnabled);
    void UpdateStats(float FrameTime, int32 ActorCount);

    static const FString& GetWidgetType() { static FString Type = "SLevelViewportToolbar"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

private:
    void OnViewModeClicked();
    void OnCameraModeClicked();
    void OnRealTimeClicked();

public:
    FString GetViewModeText(ELevelViewportType Type) const;
};

class SLevelViewport : public SVerticalBox
{
public:
    SLevelViewport();
    virtual ~SLevelViewport();

private:
    TSharedPtr<SLevelViewportToolbar> ViewportToolbar;
    TSharedPtr<SEditorViewport> EditorViewport;

    ELevelViewportType ViewportType = ELevelViewportType::Perspective;
    ULevel* CurrentLevel = nullptr;
    UWorld* CurrentWorld = nullptr;

    bool bShowToolbar = true;
    bool bShowGrid = true;
    bool bShowActorInfo = false;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime) override;

    void Initialize(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer);
    void Shutdown();

    void SetLevel(ULevel* InLevel);
    void SetWorld(UWorld* InWorld);
    void SetViewportType(ELevelViewportType InType);

    ULevel* GetLevel() const { return CurrentLevel; }
    UWorld* GetWorld() const { return CurrentWorld; }
    ELevelViewportType GetViewportType() const { return ViewportType; }

    TSharedPtr<SEditorViewport> GetEditorViewport() const { return EditorViewport; }
    TSharedPtr<SLevelViewportToolbar> GetToolbar() const { return ViewportToolbar; }

    void SetShowToolbar(bool bShow);
    void SetShowGrid(bool bShow) { bShowGrid = bShow; }
    void SetShowActorInfo(bool bShow) { bShowActorInfo = bShow; }

    bool ShouldShowToolbar() const { return bShowToolbar; }
    bool ShouldShowGrid() const { return bShowGrid; }
    bool ShouldShowActorInfo() const { return bShowActorInfo; }

    void FocusOnSelection();
    void ResetCamera();

    static const FString& GetWidgetType() { static FString Type = "SLevelViewport"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

protected:
    void CreateViewportToolbar();
    void CreateEditorViewport();
    void UpdateViewportForLevel();
    void UpdateToolbarStats();

private:
    void OnViewportTypeChanged();
    void ConfigureViewportForType(ELevelViewportType Type);
};