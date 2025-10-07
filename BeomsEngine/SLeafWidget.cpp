#include "pch.h"
#include "SLeafWidget.h"
#include <iostream>

SLeafWidget::SLeafWidget()
{
    bCanHaveChildren = false;
}

FVector2 SLeafWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return LeafDesiredSize * LayoutScaleMultiplier;
}

void SLeafWidget::SetDesiredSize(const FVector2& InDesiredSize)
{
    LeafDesiredSize = InDesiredSize;
    InvalidateLayout();
}

SButton::SButton()
{
    LeafDesiredSize = FVector2(100.0f, 30.0f);
}

FVector2 SButton::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    float TextWidth = ButtonText.size() * 8.0f;
    float MinWidth = FMath::Max(TextWidth + 20.0f, 80.0f);
    return FVector2(MinWidth, 30.0f) * LayoutScaleMultiplier;
}

int32 SButton::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const
{
    return SLeafWidget::OnPaint(Args, AllottedGeometry, InDeltaTime);
}

bool SButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(0))
    {
        bIsPressed = true;
        return true;
    }
    return false;
}

bool SButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (bIsPressed)
    {
        bIsPressed = false;
        if (MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
        {
            ExecuteOnClick();
        }
        return true;
    }
    return false;
}

bool SButton::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    bool bWasHovered = bIsHovered;
    bIsHovered = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());

    return bIsHovered;
}

void SButton::ExecuteOnClick()
{
    if (OnClicked.IsBound())
    {
        OnClicked.Execute();
    }
}

STextBlock::STextBlock()
{
    LeafDesiredSize = FVector2(50.0f, 20.0f);
}

FVector2 STextBlock::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return CalculateTextSize() * LayoutScaleMultiplier;
}

int32 STextBlock::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const
{
    return SLeafWidget::OnPaint(Args, AllottedGeometry, InDeltaTime);
}

FVector2 STextBlock::CalculateTextSize() const
{
    float CharacterWidth = FontSize * 0.6f;
    float TextWidth = Text.size() * CharacterWidth;
    float TextHeight = FontSize * 1.2f;

    return FVector2(TextWidth, TextHeight);
}