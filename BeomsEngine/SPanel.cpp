#include "pch.h"
#include "SPanel.h"
#include <iostream>

SPanel::SPanel()
{
    bCanHaveChildren = true;
}

void SPanel::ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const
{
    ArrangedChildren = Children;
}

void SPanel::OnArrangeChildren(const FGeometry& AllottedGeometry)
{
    TArray<FSlot> ArrangedChildren;
    ArrangeChildren(AllottedGeometry, ArrangedChildren);

    for (const FSlot& ChildSlot : ArrangedChildren)
    {
        if (ChildSlot.Widget)
        {
            FGeometry ChildGeometry(
                AllottedGeometry.GetAbsolutePosition() + ChildSlot.Padding,
                AllottedGeometry.GetLocalSize() - (ChildSlot.Padding * 2.0f),
                AllottedGeometry.GetScale()
            );

            ChildSlot.Widget->OnArrangeChildren(ChildGeometry);
        }
    }
}

FSlot& SPanel::AddSlot()
{
    Children.emplace_back();
    InvalidateChildOrder();
    return Children.back();
}

bool SPanel::RemoveSlot(TSharedPtr<SWidget> SlotWidget)
{
    for (auto It = Children.begin(); It != Children.end(); ++It)
    {
        if (It->Widget == SlotWidget)
        {
            Children.erase(It);
            InvalidateChildOrder();
            return true;
        }
    }
    return false;
}

void SPanel::ClearChildren()
{
    Children.clear();
    InvalidateChildOrder();
}

SHorizontalBox::SHorizontalBox()
{
}

FVector2 SHorizontalBox::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    FVector2 TotalDesiredSize = FVector2::Zero;

    for (const FSlot& ChildSlot : HorizontalSlots)
    {
        if (ChildSlot.Widget)
        {
            FVector2 ChildDesiredSize = ChildSlot.Widget->GetDesiredSize();
            TotalDesiredSize.X += ChildDesiredSize.X + ChildSlot.Padding.X * 2.0f;
            TotalDesiredSize.Y = FMath::Max(TotalDesiredSize.Y, ChildDesiredSize.Y + ChildSlot.Padding.Y * 2.0f);
        }
    }

    return TotalDesiredSize * LayoutScaleMultiplier;
}

void SHorizontalBox::ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const
{
    ArrangedChildren.clear();

    float AvailableWidth = AllottedGeometry.GetLocalSize().X;
    float CurrentX = 0.0f;

    for (const FSlot& ChildSlot : HorizontalSlots)
    {
        if (ChildSlot.Widget)
        {
            FVector2 ChildDesiredSize = ChildSlot.Widget->GetDesiredSize();
            float ChildWidth = ChildDesiredSize.X;

            if (!ChildSlot.bAutoWidth && ChildSlot.FillWidth > 0.0f)
            {
                ChildWidth = ChildSlot.FillWidth;
            }

            if (ChildSlot.MaxWidth > 0.0f)
            {
                ChildWidth = FMath::Min(ChildWidth, ChildSlot.MaxWidth);
            }

            ::FSlot ArrangedSlot;
            ArrangedSlot.Widget = ChildSlot.Widget;
            ArrangedSlot.Padding = FVector2(CurrentX + ChildSlot.Padding.X, ChildSlot.Padding.Y);
            ArrangedChildren.push_back(ArrangedSlot);

            CurrentX += ChildWidth + ChildSlot.Padding.X * 2.0f;
        }
    }
}

SHorizontalBox::FSlot& SHorizontalBox::AddSlot()
{
    HorizontalSlots.emplace_back();
    InvalidateChildOrder();
    return HorizontalSlots.back();
}

bool SHorizontalBox::RemoveSlot(TSharedPtr<SWidget> SlotWidget)
{
    for (auto It = HorizontalSlots.begin(); It != HorizontalSlots.end(); ++It)
    {
        if (It->Widget == SlotWidget)
        {
            HorizontalSlots.erase(It);
            InvalidateChildOrder();
            return true;
        }
    }
    return false;
}

void SHorizontalBox::ClearSlots()
{
    HorizontalSlots.clear();
    InvalidateChildOrder();
}

SVerticalBox::SVerticalBox()
{
}

FVector2 SVerticalBox::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    FVector2 TotalDesiredSize = FVector2::Zero;

    for (const FSlot& ChildSlot : VerticalSlots)
    {
        if (ChildSlot.Widget)
        {
            FVector2 ChildDesiredSize = ChildSlot.Widget->GetDesiredSize();
            TotalDesiredSize.X = FMath::Max(TotalDesiredSize.X, ChildDesiredSize.X + ChildSlot.Padding.X * 2.0f);
            TotalDesiredSize.Y += ChildDesiredSize.Y + ChildSlot.Padding.Y * 2.0f;
        }
    }

    return TotalDesiredSize * LayoutScaleMultiplier;
}

void SVerticalBox::ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const
{
    ArrangedChildren.clear();

    float AvailableHeight = AllottedGeometry.GetLocalSize().Y;
    float CurrentY = 0.0f;

    for (const FSlot& ChildSlot : VerticalSlots)
    {
        if (ChildSlot.Widget)
        {
            FVector2 ChildDesiredSize = ChildSlot.Widget->GetDesiredSize();
            float ChildHeight = ChildDesiredSize.Y;

            if (!ChildSlot.bAutoHeight && ChildSlot.FillHeight > 0.0f)
            {
                ChildHeight = ChildSlot.FillHeight;
            }

            if (ChildSlot.MaxHeight > 0.0f)
            {
                ChildHeight = FMath::Min(ChildHeight, ChildSlot.MaxHeight);
            }

            ::FSlot ArrangedSlot;
            ArrangedSlot.Widget = ChildSlot.Widget;
            ArrangedSlot.Padding = FVector2(ChildSlot.Padding.X, CurrentY + ChildSlot.Padding.Y);
            ArrangedChildren.push_back(ArrangedSlot);

            CurrentY += ChildHeight + ChildSlot.Padding.Y * 2.0f;
        }
    }
}

SVerticalBox::FSlot& SVerticalBox::AddSlot()
{
    VerticalSlots.emplace_back();
    InvalidateChildOrder();
    return VerticalSlots.back();
}

bool SVerticalBox::RemoveSlot(TSharedPtr<SWidget> SlotWidget)
{
    for (auto It = VerticalSlots.begin(); It != VerticalSlots.end(); ++It)
    {
        if (It->Widget == SlotWidget)
        {
            VerticalSlots.erase(It);
            InvalidateChildOrder();
            return true;
        }
    }
    return false;
}

void SVerticalBox::ClearSlots()
{
    VerticalSlots.clear();
    InvalidateChildOrder();
}