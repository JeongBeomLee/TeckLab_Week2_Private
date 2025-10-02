#include "pch.h"
#include "SLevelViewport.h"
#include "World.h"
#include "Level.h"
#include <iostream>

SLevelViewportToolbar::SLevelViewportToolbar()
{
    Initialize();
}

void SLevelViewportToolbar::Initialize()
{
    ViewModeButton = std::make_shared<SButton>();
    ViewModeButton->SetText("Perspective");
    ViewModeButton->OnClicked.BindRaw(this, &SLevelViewportToolbar::OnViewModeClicked);

    CameraModeButton = std::make_shared<SButton>();
    CameraModeButton->SetText("Orbit");
    CameraModeButton->OnClicked.BindRaw(this, &SLevelViewportToolbar::OnCameraModeClicked);

    RealTimeButton = std::make_shared<SButton>();
    RealTimeButton->SetText("Realtime");
    RealTimeButton->OnClicked.BindRaw(this, &SLevelViewportToolbar::OnRealTimeClicked);

    ViewportStatsText = std::make_shared<STextBlock>();
    ViewportStatsText->SetText("Stats: 0ms | 0 actors");

    AddSlot().SetPadding(FVector2(5.0f, 2.0f))[ViewModeButton];
    AddSlot().SetPadding(FVector2(5.0f, 2.0f))[CameraModeButton];
    AddSlot().SetPadding(FVector2(5.0f, 2.0f))[RealTimeButton];
    AddSlot().SetFillWidth(1.0f).SetPadding(FVector2(10.0f, 2.0f))[ViewportStatsText];

    printf("   SLevelViewportToolbar initialized with buttons and stats\n");
}

void SLevelViewportToolbar::SetViewportType(ELevelViewportType InType)
{
    if (ViewModeButton)
    {
        ViewModeButton->SetText(GetViewModeText(InType));
    }
}

void SLevelViewportToolbar::SetRealTimeEnabled(bool bEnabled)
{
    if (RealTimeButton)
    {
        RealTimeButton->SetText(bEnabled ? "Realtime" : "Paused");
    }
}

void SLevelViewportToolbar::UpdateStats(float FrameTime, int32 ActorCount)
{
    if (ViewportStatsText)
    {
        char StatsBuffer[128];
        snprintf(StatsBuffer, sizeof(StatsBuffer), "Stats: %.1fms | %d actors", FrameTime * 1000.0f, ActorCount);
        ViewportStatsText->SetText(StatsBuffer);
    }
}

void SLevelViewportToolbar::OnViewModeClicked()
{
    printf("   ViewMode button clicked\n");
}

void SLevelViewportToolbar::OnCameraModeClicked()
{
    printf("   CameraMode button clicked\n");
}

void SLevelViewportToolbar::OnRealTimeClicked()
{
    printf("   RealTime button clicked\n");
}

FString SLevelViewportToolbar::GetViewModeText(ELevelViewportType Type) const
{
    switch (Type)
    {
    case ELevelViewportType::Perspective: return "Perspective";
    case ELevelViewportType::Orthographic_Top: return "Top";
    case ELevelViewportType::Orthographic_Side: return "Side";
    case ELevelViewportType::Orthographic_Front: return "Front";
    default: return "Unknown";
    }
}

SLevelViewport::SLevelViewport()
{
    CreateViewportToolbar();
    CreateEditorViewport();

    printf("   SLevelViewport created with toolbar and editor viewport\n");
}

SLevelViewport::~SLevelViewport()
{
    Shutdown();
}

FVector2 SLevelViewport::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    FVector2 ToolbarSize = FVector2::Zero;
    FVector2 ViewportSize = FVector2(800.0f, 600.0f);

    if (bShowToolbar && ViewportToolbar)
    {
        ToolbarSize = ViewportToolbar->GetDesiredSize();
        ViewportSize.Y -= ToolbarSize.Y;
    }

    FVector2 TotalSize = FVector2(
        FMath::Max(ToolbarSize.X, ViewportSize.X),
        ToolbarSize.Y + ViewportSize.Y
    );

    return TotalSize * LayoutScaleMultiplier;
}

void SLevelViewport::Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime)
{
    UpdateToolbarStats();
    SVerticalBox::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SLevelViewport::Initialize(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer)
{
    if (EditorViewport)
    {
        EditorViewport->Initialize(GraphicsDevice, Renderer);
    }

    printf("   SLevelViewport initialized with GraphicsDevice and Renderer\n");
}

void SLevelViewport::Shutdown()
{
    if (EditorViewport)
    {
        EditorViewport->Shutdown();
    }

    CurrentLevel = nullptr;
    CurrentWorld = nullptr;

    printf("   SLevelViewport shutdown completed\n");
}

void SLevelViewport::SetLevel(ULevel* InLevel)
{
    CurrentLevel = InLevel;
    UpdateViewportForLevel();

    if (CurrentLevel)
    {
        printf("   SLevelViewport set to level: %s\n", CurrentLevel->GetLevelName().ToString().c_str());
    }
    else
    {
        printf("   SLevelViewport level cleared\n");
    }
}

void SLevelViewport::SetWorld(UWorld* InWorld)
{
    CurrentWorld = InWorld;

    if (CurrentWorld)
    {
        printf("   SLevelViewport set to world: %s\n", CurrentWorld->GetWorldName().ToString().c_str());
        SetLevel(CurrentWorld->GetCurrentLevel());
    }
    else
    {
        printf("   SLevelViewport world cleared\n");
        SetLevel(nullptr);
    }
}

void SLevelViewport::SetViewportType(ELevelViewportType InType)
{
    if (ViewportType != InType)
    {
        ViewportType = InType;
        ConfigureViewportForType(InType);
        OnViewportTypeChanged();

        if (ViewportToolbar)
        {
            ViewportToolbar->SetViewportType(InType);
        }

        printf("   SLevelViewport type changed to: %s\n",
            GetViewModeText(InType).c_str());
    }
}

void SLevelViewport::SetShowToolbar(bool bShow)
{
    if (bShowToolbar != bShow)
    {
        bShowToolbar = bShow;

        if (ViewportToolbar)
        {
            ViewportToolbar->SetVisibility(bShow ? EVisibility::Visible : EVisibility::Collapsed);
        }

        InvalidateLayout();
        printf("   SLevelViewport toolbar %s\n", bShow ? "shown" : "hidden");
    }
}

void SLevelViewport::FocusOnSelection()
{
    if (EditorViewport && EditorViewport->GetViewportClient())
    {
        EditorViewport->GetViewportClient()->FocusOnPoint(FVector::Zero);
        printf("   SLevelViewport focused on selection\n");
    }
}

void SLevelViewport::ResetCamera()
{
    if (EditorViewport && EditorViewport->GetViewportClient())
    {
        EditorViewport->GetViewportClient()->ResetCamera();
        printf("   SLevelViewport camera reset\n");
    }
}

void SLevelViewport::CreateViewportToolbar()
{
    if (bShowToolbar)
    {
        ViewportToolbar = std::make_shared<SLevelViewportToolbar>();
        AddSlot().SetAutoHeight(true).SetPadding(FVector2(0.0f, 0.0f))[ViewportToolbar];
    }
}

void SLevelViewport::CreateEditorViewport()
{
    EditorViewport = std::make_shared<SEditorViewport>();
    AddSlot().SetFillHeight(1.0f).SetPadding(FVector2(0.0f, 0.0f))[EditorViewport];
}
void SLevelViewport::UpdateViewportForLevel()
{
    if (EditorViewport && EditorViewport->GetViewportClient())
    {
        if (CurrentLevel)
        {
            FocusOnSelection();
        }
    }
}

void SLevelViewport::UpdateToolbarStats()
{
    if (ViewportToolbar)
    {
        float FrameTime = EditorViewport ? EditorViewport->GetLastRenderTime() : 0.0f;
        int32 ActorCount = CurrentLevel ? CurrentLevel->GetNumActors() : 0;

        ViewportToolbar->UpdateStats(FrameTime, ActorCount);
    }
}

void SLevelViewport::OnViewportTypeChanged()
{
    UpdateViewportForLevel();
}

void SLevelViewport::ConfigureViewportForType(ELevelViewportType Type)
{
    if (EditorViewport && EditorViewport->GetViewportClient())
    {
        auto ViewportClient = EditorViewport->GetViewportClient();

        switch (Type)
        {
        case ELevelViewportType::Perspective:
            ViewportClient->SetProjectionType(EProjectionType::Perspective);
            ViewportClient->SetCameraMode(ECameraMode::Orbit);
            ViewportClient->SetFOV(75.0f);
            break;

        case ELevelViewportType::Orthographic_Top:
        case ELevelViewportType::Orthographic_Side:
        case ELevelViewportType::Orthographic_Front:
            ViewportClient->SetProjectionType(EProjectionType::Orthographic);
            ViewportClient->SetCameraMode(ECameraMode::Free);
            break;
        }
    }
}

FString SLevelViewport::GetViewModeText(ELevelViewportType Type) const
{
    switch (Type)
    {
    case ELevelViewportType::Perspective: return "Perspective";
    case ELevelViewportType::Orthographic_Top: return "Top";
    case ELevelViewportType::Orthographic_Side: return "Side";
    case ELevelViewportType::Orthographic_Front: return "Front";
    default: return "Unknown";
    }
}