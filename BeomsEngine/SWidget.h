#pragma once
#include "SlateCore.h"

class SWidget
{
public:
    SWidget();
    virtual ~SWidget() = default;

protected:
    EVisibility Visibility = EVisibility::Visible;
    bool bEnabled = true;
    bool bCanHaveChildren = false;
    FVector2 DesiredSize = FVector2::Zero;
    bool bNeedsDesiredSizeCompute = true;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const = 0;
    virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const {}
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const;

    virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) { return false; }
    virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) { return false; }
    virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) { return false; }
    virtual bool OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) { return false; }

    virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime) {}

    void SetVisibility(EVisibility InVisibility) { Visibility = InVisibility; }
    EVisibility GetVisibility() const { return Visibility; }

    void SetEnabled(bool bInEnabled) { bEnabled = bInEnabled; }
    bool IsEnabled() const { return bEnabled; }

    bool CanHaveChildren() const { return bCanHaveChildren; }

    FVector2 GetDesiredSize() const;
    void SlatePrepass(float LayoutScaleMultiplier = 1.0f);
    void InvalidateLayout() { bNeedsDesiredSizeCompute = true; }

    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry) {}

    static const FString& GetWidgetType() { static FString Type = "SWidget"; return Type; }
    virtual const FString& GetType() const { return GetWidgetType(); }

protected:
    virtual void CacheDesiredSize(float LayoutScaleMultiplier);

private:
    mutable FVector2 CachedDesiredSize = FVector2::Zero;
    mutable bool bCachedDesiredSizeValid = false;

public:
    static bool IsConstructed() { return true; }
};