#include "pch.h"
#include "PrimitiveComponent.h"

IMPLEMENT_CLASS(UPrimitiveComponent, USceneComponent)

UPrimitiveComponent::UPrimitiveComponent()
    : bVisible(true)
    , bHidden(false)
{
}

UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::BeginPlay()
{
    Super::BeginPlay();

    // 렌더링 상태 초기화
    UpdateRenderState();
}

void UPrimitiveComponent::EndPlay()
{
    Super::EndPlay();
}

void UPrimitiveComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UPrimitiveComponent::SetVisibility(bool bNewVisibility)
{
    if (bVisible != bNewVisibility)
    {
        bVisible = bNewVisibility;
        MarkRenderStateDirty();
    }
}

void UPrimitiveComponent::SetHidden(bool bNewHidden)
{
    if (bHidden != bNewHidden)
    {
        bHidden = bNewHidden;
        MarkRenderStateDirty();
    }
}

FVector UPrimitiveComponent::GetBoundingBoxCenter() const
{
    FVector Min = GetBoundingBoxMin();
    FVector Max = GetBoundingBoxMax();
    return (Min + Max) * 0.5f;
}

FVector UPrimitiveComponent::GetBoundingBoxExtent() const
{
    FVector Min = GetBoundingBoxMin();
    FVector Max = GetBoundingBoxMax();
    return (Max - Min) * 0.5f;
}

FBoxSphereBounds UPrimitiveComponent::GetBounds() const
{
    FVector Min = GetBoundingBoxMin();
    FVector Max = GetBoundingBoxMax();
    FVector Origin = (Min + Max) * 0.5f;
    FVector BoxExtent = (Max - Min) * 0.5f;
    float SphereRadius = GetBoundingSphereRadius();

    return FBoxSphereBounds(Origin, BoxExtent, SphereRadius);
}

bool UPrimitiveComponent::LineTraceComponent(const FVector& Start, const FVector& End, FVector& OutHitLocation) const
{
    // 기본 구현: 바운딩 박스와의 교차 검사
    FVector BoxMin = GetBoundingBoxMin();
    FVector BoxMax = GetBoundingBoxMax();

    // 간단한 AABB 레이 교차 테스트
    FVector InvDir = FVector(
        1.0f / (End.X - Start.X),
        1.0f / (End.Y - Start.Y),
        1.0f / (End.Z - Start.Z)
    );

    float t1 = (BoxMin.X - Start.X) * InvDir.X;
    float t2 = (BoxMax.X - Start.X) * InvDir.X;
    float t3 = (BoxMin.Y - Start.Y) * InvDir.Y;
    float t4 = (BoxMax.Y - Start.Y) * InvDir.Y;
    float t5 = (BoxMin.Z - Start.Z) * InvDir.Z;
    float t6 = (BoxMax.Z - Start.Z) * InvDir.Z;

    float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
    float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

    if (tmax < 0 || tmin > tmax || tmin > 1.0f)
    {
        return false;
    }

    float t = tmin > 0 ? tmin : tmax;
    OutHitLocation = Start + (End - Start) * t;
    return true;
}
