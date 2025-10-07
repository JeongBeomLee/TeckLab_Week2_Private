#include "pch.h"
#include "SEditorViewport.h"
#include "EditorViewportClient.h"
#include "D3D11GraphicsDevice.h"
#include "Renderer.h"
#include <iostream>

SEditorViewport::SEditorViewport()
{
    LeafDesiredSize = FVector2(800.0f, 600.0f);
    bIsRealtime = true;
    bShowStats = false;
    LastRenderTime = 0.0f;
}

SEditorViewport::~SEditorViewport()
{
    Shutdown();
}

FVector2 SEditorViewport::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return LeafDesiredSize * LayoutScaleMultiplier;
}

int32 SEditorViewport::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const
{
    if (bIsRealtime)
    {
        const_cast<SEditorViewport*>(this)->RenderViewport(AllottedGeometry, InDeltaTime);
    }

    return SLeafWidget::OnPaint(Args, AllottedGeometry, InDeltaTime);
}

void SEditorViewport::Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime)
{
    UpdateViewportClient(AllottedGeometry, InDeltaTime);

    if (!bIsRealtime)
    {
        return;
    }

    SLeafWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

bool SEditorViewport::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ViewportClient)
    {
        FViewportInputEvent ViewportEvent;
        ViewportEvent.MouseX = (int32)MouseEvent.GetScreenSpacePosition().X;
        ViewportEvent.MouseY = (int32)MouseEvent.GetScreenSpacePosition().Y;
        ViewportEvent.bIsPressed = true;

        if (MouseEvent.IsMouseButtonDown(0)) ViewportEvent.Button = EMouseButton::Left;
        else if (MouseEvent.IsMouseButtonDown(1)) ViewportEvent.Button = EMouseButton::Right;
        else if (MouseEvent.IsMouseButtonDown(2)) ViewportEvent.Button = EMouseButton::Middle;

        ViewportClient->OnMouseButtonDown(ViewportEvent);
        return true;
    }

    return SLeafWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
}

bool SEditorViewport::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ViewportClient)
    {
        FViewportInputEvent ViewportEvent;
        ViewportEvent.MouseX = (int32)MouseEvent.GetScreenSpacePosition().X;
        ViewportEvent.MouseY = (int32)MouseEvent.GetScreenSpacePosition().Y;
        ViewportEvent.bIsPressed = false;

        if (MouseEvent.IsMouseButtonDown(0)) ViewportEvent.Button = EMouseButton::Left;
        else if (MouseEvent.IsMouseButtonDown(1)) ViewportEvent.Button = EMouseButton::Right;
        else if (MouseEvent.IsMouseButtonDown(2)) ViewportEvent.Button = EMouseButton::Middle;

        ViewportClient->OnMouseButtonUp(ViewportEvent);
        return true;
    }

    return SLeafWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
}

bool SEditorViewport::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ViewportClient)
    {
        FViewportInputEvent ViewportEvent;
        ViewportEvent.MouseX = (int32)MouseEvent.GetScreenSpacePosition().X;
        ViewportEvent.MouseY = (int32)MouseEvent.GetScreenSpacePosition().Y;
        ViewportEvent.MouseDeltaX = (int32)MouseEvent.GetCursorDelta().X;
        ViewportEvent.MouseDeltaY = (int32)MouseEvent.GetCursorDelta().Y;

        ViewportClient->OnMouseMove(ViewportEvent);
        return true;
    }

    return SLeafWidget::OnMouseMove(MyGeometry, MouseEvent);
}

bool SEditorViewport::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (ViewportClient)
    {
        return true;
    }

    return SLeafWidget::OnMouseWheel(MyGeometry, MouseEvent);
}

void SEditorViewport::Initialize(FD3D11GraphicsDevice* InGraphicsDevice, URenderer* InRenderer)
{
    GraphicsDevice = InGraphicsDevice;
    Renderer = InRenderer;

    if (!ViewportClient)
    {
        ViewportClient = std::make_shared<FEditorViewportClient>();
        ViewportClient->Initialize(
            (uint32)LeafDesiredSize.X,
            (uint32)LeafDesiredSize.Y
        );
    }
}

void SEditorViewport::Shutdown()
{
    if (ViewportClient)
    {
        ViewportClient->Shutdown();
        ViewportClient.reset();
    }

    GraphicsDevice = nullptr;
    Renderer = nullptr;
}

void SEditorViewport::SetViewportClient(TSharedPtr<FEditorViewportClient> InViewportClient)
{
    if (ViewportClient)
    {
        ViewportClient->Shutdown();
    }

    ViewportClient = InViewportClient;

    if (ViewportClient)
    {
        ViewportClient->Initialize(
            (uint32)LeafDesiredSize.X,
            (uint32)LeafDesiredSize.Y
        );
    }
}

void SEditorViewport::RenderViewport(const FGeometry& AllottedGeometry, float DeltaTime)
{
    if (!ViewportClient || !GraphicsDevice || !Renderer)
    {
        return;
    }

    float StartTime = 0.0f; // 실제로는 현재 시간을 가져와야 함

    ViewportClient->SetViewportSize(
        (uint32)AllottedGeometry.GetLocalSize().X,
        (uint32)AllottedGeometry.GetLocalSize().Y
    );

    ViewportClient->Draw(GraphicsDevice, Renderer);

    float EndTime = StartTime + DeltaTime;
    LastRenderTime = EndTime - StartTime;
}

void SEditorViewport::UpdateViewportClient(const FGeometry& AllottedGeometry, float DeltaTime)
{
    if (ViewportClient)
    {
        ViewportClient->Tick(DeltaTime);
    }
}

FPointerEvent SEditorViewport::ConvertToViewportEvent(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent) const
{
    FPointerEvent ViewportEvent = InMouseEvent;

    FVector2 LocalMousePos = InMouseEvent.GetScreenSpacePosition() - MyGeometry.GetAbsolutePosition();
    ViewportEvent.ScreenSpacePosition = LocalMousePos;

    return ViewportEvent;
}