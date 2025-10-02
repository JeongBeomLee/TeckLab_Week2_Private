#pragma once
#include "SLeafWidget.h"
#include "ViewportClient.h"

class FD3D11GraphicsDevice;
class URenderer;
class FEditorViewportClient;

class SEditorViewport : public SLeafWidget
{
public:
    SEditorViewport();
    virtual ~SEditorViewport();

protected:
    TSharedPtr<FEditorViewportClient> ViewportClient;
    FD3D11GraphicsDevice* GraphicsDevice = nullptr;
    URenderer* Renderer = nullptr;

    bool bIsRealtime = true;
    bool bShowStats = false;
    float LastRenderTime = 0.0f;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;
    virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime) override;

    virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual bool OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    void Initialize(FD3D11GraphicsDevice* InGraphicsDevice, URenderer* InRenderer);
    void Shutdown();

    void SetViewportClient(TSharedPtr<FEditorViewportClient> InViewportClient);
    TSharedPtr<FEditorViewportClient> GetViewportClient() const { return ViewportClient; }

    void SetRealtime(bool bInRealtime) { bIsRealtime = bInRealtime; }
    bool IsRealtime() const { return bIsRealtime; }

    void SetShowStats(bool bInShowStats) { bShowStats = bInShowStats; }
    bool ShouldShowStats() const { return bShowStats; }

    float GetLastRenderTime() const { return LastRenderTime; }

    static const FString& GetWidgetType() { static FString Type = "SEditorViewport"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

protected:
    void RenderViewport(const FGeometry& AllottedGeometry, float DeltaTime);
    void UpdateViewportClient(const FGeometry& AllottedGeometry, float DeltaTime);

private:
    FPointerEvent ConvertToViewportEvent(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent) const;
};