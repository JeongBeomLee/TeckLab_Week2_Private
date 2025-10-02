#pragma once
#include "SWidget.h"

class SPanel : public SWidget
{
public:
    SPanel();
    virtual ~SPanel() = default;

protected:
    TArray<FSlot> Children;

public:
    virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const override;
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry) override;

    int32 GetChildrenCount() const { return static_cast<int32>(Children.size()); }
    TArray<FSlot>& GetChildren() { return Children; }
    const TArray<FSlot>& GetChildren() const { return Children; }

    virtual FSlot& AddSlot();
    virtual bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
    virtual void ClearChildren();

    static const FString& GetWidgetType() { static FString Type = "SPanel"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }

protected:
    void InvalidateChildOrder() { InvalidateLayout(); }
};

class SHorizontalBox : public SPanel
{
public:
    struct FSlot : public FSlotBase
    {
        TSharedPtr<SWidget> Widget;
        float FillWidth = 0.0f;
        float MaxWidth = 0.0f;
        bool bAutoWidth = true;

        FSlot() = default;
        FSlot(TSharedPtr<SWidget> InWidget) : Widget(InWidget) {}

        FSlot& SetFillWidth(float InFillWidth) { FillWidth = InFillWidth; bAutoWidth = false; return *this; }
        FSlot& SetMaxWidth(float InMaxWidth) { MaxWidth = InMaxWidth; return *this; }
        FSlot& SetAutoWidth(bool bInAutoWidth) { bAutoWidth = bInAutoWidth; return *this; }
    };

private:
    TArray<FSlot> HorizontalSlots;

public:
    SHorizontalBox();
    virtual ~SHorizontalBox() = default;

    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const override;

    FSlot& AddSlot();
    bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
    void ClearSlots();

	int32 GetSlotCount() const { return static_cast<int32>(HorizontalSlots.size()); }
    const TArray<FSlot>& GetSlots() const { return HorizontalSlots; }

    static const FString& GetWidgetType() { static FString Type = "SHorizontalBox"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }
};

class SVerticalBox : public SPanel
{
public:
    struct FSlot : public FSlotBase
    {
        TSharedPtr<SWidget> Widget;
        float FillHeight = 0.0f;
        float MaxHeight = 0.0f;
        bool bAutoHeight = true;

        FSlot() = default;
        FSlot(TSharedPtr<SWidget> InWidget) : Widget(InWidget) {}

        FSlot& SetFillHeight(float InFillHeight) { FillHeight = InFillHeight; bAutoHeight = false; return *this; }
        FSlot& SetMaxHeight(float InMaxHeight) { MaxHeight = InMaxHeight; return *this; }
        FSlot& SetAutoHeight(bool bInAutoHeight) { bAutoHeight = bInAutoHeight; return *this; }
    };

private:
    TArray<FSlot> VerticalSlots;

public:
    SVerticalBox();
    virtual ~SVerticalBox() = default;

    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
    virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const override;

    FSlot& AddSlot();
    bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
    void ClearSlots();

    int32 GetSlotCount() const { return static_cast<int32>(VerticalSlots.size()); }
    const TArray<FSlot>& GetSlots() const { return VerticalSlots; }

    static const FString& GetWidgetType() { static FString Type = "SVerticalBox"; return Type; }
    virtual const FString& GetType() const override { return GetWidgetType(); }
};