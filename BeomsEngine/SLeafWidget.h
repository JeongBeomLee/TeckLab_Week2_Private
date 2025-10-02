#pragma once
#include "SWidget.h"
#include "Delegate.h"

class SLeafWidget : public SWidget
{
public:
    SLeafWidget();
    virtual ~SLeafWidget() = default;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const final override {}

    static const FString& GetWidgetType() { static FString Type = "SLeafWidget"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

protected:
    FVector2 LeafDesiredSize = FVector2(100.0f, 30.0f);

public:
    void SetDesiredSize(const FVector2& InDesiredSize);
    FVector2 GetLeafDesiredSize() const { return LeafDesiredSize; }
};

class SButton : public SLeafWidget
{
public:
    SButton();
    virtual ~SButton() = default;

private:
    FString ButtonText = "Button";
    bool bIsPressed = false;
    bool bIsHovered = false;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;

    virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    void SetText(const FString& InText) { ButtonText = InText; }
    FString GetText() const { return ButtonText; }

    bool IsPressed() const { return bIsPressed; }
    bool IsHovered() const { return bIsHovered; }

    static const FString& GetWidgetType() { static FString Type = "SButton"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

public:
    DECLARE_DELEGATE(FOnClicked)
    FOnClicked OnClicked;

protected:
    void ExecuteOnClick();
};

class STextBlock : public SLeafWidget
{
public:
    STextBlock();
    virtual ~STextBlock() = default;

private:
    FString Text = "Text";
    float FontSize = 12.0f;

public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;

    void SetText(const FString& InText) { Text = InText; InvalidateLayout(); }
    FString GetText() const { return Text; }

    void SetFontSize(float InFontSize) { FontSize = InFontSize; InvalidateLayout(); }
    float GetFontSize() const { return FontSize; }

    static const FString& GetWidgetType() { static FString Type = "STextBlock"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

private:
    FVector2 CalculateTextSize() const;
};