#include "pch.h"
#include "SWidget.h"

SWidget::SWidget()
{
    Visibility = EVisibility::Visible;
    bEnabled = true;
    bCanHaveChildren = false;
    DesiredSize = FVector2::Zero;
    bNeedsDesiredSizeCompute = true;
}

int32 SWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const
{
    return 0;
}

FVector2 SWidget::GetDesiredSize() const
{
    if (bCachedDesiredSizeValid)
    {
        return CachedDesiredSize;
    }

    return DesiredSize;
}

void SWidget::SlatePrepass(float LayoutScaleMultiplier)
{
    if (bNeedsDesiredSizeCompute)
    {
        CacheDesiredSize(LayoutScaleMultiplier);
        bNeedsDesiredSizeCompute = false;
    }
}

void SWidget::CacheDesiredSize(float LayoutScaleMultiplier)
{
    CachedDesiredSize = ComputeDesiredSize(LayoutScaleMultiplier);
    bCachedDesiredSizeValid = true;
}