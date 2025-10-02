#pragma once
#include "Types.h"
#include "Math.h"
#include "Vector2.h"
#include "Containers.h"
#include <memory>

enum class EVisibility : uint8
{
    Visible,    // 존재
	Collapsed,  // 없음, 공간도 차지하지 않음
	Hidden      // 없음, 공간은 차지함
};

enum class EHorizontalAlignment : uint8
{
    HAlign_Fill,
    HAlign_Left,
    HAlign_Center,
    HAlign_Right
};

enum class EVerticalAlignment : uint8
{
    VAlign_Fill,
    VAlign_Top,
    VAlign_Center,
    VAlign_Bottom
};

struct FGeometry
{
    FVector2 AbsolutePosition;
    FVector2 LocalSize;
    FVector2 AccumulatedRenderTransform;
    float Scale;

    FGeometry()
        : AbsolutePosition(FVector2::Zero)
        , LocalSize(FVector2::Zero)
        , AccumulatedRenderTransform(FVector2::Zero)
        , Scale(1.0f)
    {
    }

    FGeometry(const FVector2& InAbsolutePosition, const FVector2& InLocalSize, float InScale = 1.0f)
        : AbsolutePosition(InAbsolutePosition)
        , LocalSize(InLocalSize)
        , AccumulatedRenderTransform(FVector2::Zero)
        , Scale(InScale)
    {
    }

    FVector2 GetLocalSize() const { return LocalSize; }
    FVector2 GetAbsolutePosition() const { return AbsolutePosition; }
    float GetScale() const { return Scale; }

    bool IsUnderLocation(const FVector2& AbsoluteCoordinate) const
    {
        return AbsoluteCoordinate.X >= AbsolutePosition.X &&
               AbsoluteCoordinate.Y >= AbsolutePosition.Y &&
               AbsoluteCoordinate.X < (AbsolutePosition.X + LocalSize.X) &&
               AbsoluteCoordinate.Y < (AbsolutePosition.Y + LocalSize.Y);
    }
};

struct FSlotBase
{
    EHorizontalAlignment HAlignment = EHorizontalAlignment::HAlign_Fill;
    EVerticalAlignment VAlignment = EVerticalAlignment::VAlign_Fill;
    FVector2 Padding = FVector2::Zero;

    FSlotBase& SetPadding(const FVector2& InPadding) { Padding = InPadding; return *this; }
    FSlotBase& SetHAlign(EHorizontalAlignment InHAlignment) { HAlignment = InHAlignment; return *this; }
    FSlotBase& SetVAlign(EVerticalAlignment InVAlignment) { VAlignment = InVAlignment; return *this; }
};

template<typename WidgetType>
struct TSlot : public FSlotBase
{
    TSharedPtr<WidgetType> Widget;

    TSlot() = default;
    TSlot(TSharedPtr<WidgetType> InWidget) : Widget(InWidget) {}

    TSlot& operator[](TSharedPtr<WidgetType> InWidget)
    {
        Widget = InWidget;
        return *this;
    }

    TSharedPtr<WidgetType> GetWidget() const { return Widget; }
};

class SWidget;
using FSlot = TSlot<SWidget>;

struct FPaintArgs
{
    FGeometry AllottedGeometry;
    float DeltaTime;
    bool bParentEnabled;

    FPaintArgs(const FGeometry& InGeometry, float InDeltaTime, bool bInParentEnabled = true)
        : AllottedGeometry(InGeometry), DeltaTime(InDeltaTime), bParentEnabled(bInParentEnabled)
    {
    }
};

struct FPointerEvent
{
    FVector2 ScreenSpacePosition;
    FVector2 LastScreenSpacePosition;
    FVector2 CursorDelta;
    bool bIsLeftMouseButtonDown;
    bool bIsRightMouseButtonDown;
    bool bIsMiddleMouseButtonDown;

    FPointerEvent()
        : ScreenSpacePosition(FVector2::Zero)
        , LastScreenSpacePosition(FVector2::Zero)
        , CursorDelta(FVector2::Zero)
        , bIsLeftMouseButtonDown(false)
        , bIsRightMouseButtonDown(false)
        , bIsMiddleMouseButtonDown(false)
    {
    }

    FPointerEvent(const FVector2& InPosition, bool bLeftDown = false, bool bRightDown = false, bool bMiddleDown = false)
        : ScreenSpacePosition(InPosition)
        , LastScreenSpacePosition(InPosition)
        , CursorDelta(FVector2::Zero)
        , bIsLeftMouseButtonDown(bLeftDown)
        , bIsRightMouseButtonDown(bRightDown)
        , bIsMiddleMouseButtonDown(bMiddleDown)
    {
    }

    FVector2 GetScreenSpacePosition() const { return ScreenSpacePosition; }
    FVector2 GetCursorDelta() const { return CursorDelta; }
    bool IsMouseButtonDown(int32 Button) const
    {
        switch (Button)
        {
        case 0: return bIsLeftMouseButtonDown;
        case 1: return bIsRightMouseButtonDown;
        case 2: return bIsMiddleMouseButtonDown;
        default: return false;
        }
    }
};