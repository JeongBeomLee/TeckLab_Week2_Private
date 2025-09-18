#include "pch.h"
#include "SceneComponent.h"
#include "ObjectInitializer.h"
#include <algorithm>

// RTTI 매크로 구현
IMPLEMENT_CLASS(USceneComponent, UActorComponent)

// USceneComponent 구현
USceneComponent::USceneComponent()
    : UActorComponent()
    , WorldLocation(FVector::Zero)
    , WorldRotation(FVector::Zero)
    , WorldScale(FVector::One)
    , RelativeLocation(FVector::Zero)
    , RelativeRotation(FVector::Zero)
    , RelativeScale(FVector::One)
    , AttachParent(nullptr)
    , bVisible(true)
    , bAbsoluteLocation(false)
    , bAbsoluteRotation(false)
    , bAbsoluteScale(false)
{
    SetName(TEXT("SceneComponent"));
    bCanEverTick = true;
}

USceneComponent::~USceneComponent()
{
    // 부모에서 분리
    if (AttachParent)
    {
        DetachFromComponent();
    }
    
    // 자식들 분리
    while (!AttachChildren.empty())
    {
        USceneComponent* Child = AttachChildren.back();
        if (Child)
        {
            Child->DetachFromComponent();
        }
    }
}

void USceneComponent::SetWorldLocation(const FVector& NewLocation)
{
    WorldLocation = NewLocation;
    
    if (AttachParent && !bAbsoluteLocation)
    {
        // 부모가 있으면 상대 위치 계산
        RelativeLocation = WorldLocation - AttachParent->GetComponentLocation();
    }
    else
    {
        RelativeLocation = WorldLocation;
    }
    
    UpdateChildTransforms();
}

void USceneComponent::SetWorldRotation(const FVector& NewRotation)
{
    WorldRotation = NewRotation;
    
    if (AttachParent && !bAbsoluteRotation)
    {
        // 부모가 있으면 상대 회전 계산
        RelativeRotation = WorldRotation - AttachParent->GetComponentRotation();
    }
    else
    {
        RelativeRotation = WorldRotation;
    }
    
    UpdateChildTransforms();
}

void USceneComponent::SetWorldScale(const FVector& NewScale)
{
    WorldScale = NewScale;
    
    if (AttachParent && !bAbsoluteScale)
    {
        // 부모가 있으면 상대 스케일 계산
        FVector ParentScale = AttachParent->GetComponentScale();
        RelativeScale = FVector(
            ParentScale.X != 0.0f ? WorldScale.X / ParentScale.X : 1.0f,
            ParentScale.Y != 0.0f ? WorldScale.Y / ParentScale.Y : 1.0f,
            ParentScale.Z != 0.0f ? WorldScale.Z / ParentScale.Z : 1.0f
        );
    }
    else
    {
        RelativeScale = WorldScale;
    }
    
    UpdateChildTransforms();
}

void USceneComponent::SetRelativeLocation(const FVector& NewLocation)
{
    RelativeLocation = NewLocation;
    UpdateWorldTransform();
}

void USceneComponent::SetRelativeRotation(const FVector& NewRotation)
{
    RelativeRotation = NewRotation;
    UpdateWorldTransform();
}

void USceneComponent::SetRelativeScale(const FVector& NewScale)
{
    RelativeScale = NewScale;
    UpdateWorldTransform();
}

void USceneComponent::AttachToComponent(USceneComponent* Parent)
{
    if (Parent && Parent != this && Parent != AttachParent)
    {
        // 기존 부모에서 분리
        if (AttachParent)
        {
            DetachFromComponent();
        }
        
        // 새 부모에 연결
        AttachParent = Parent;
        Parent->AddChild(this);
        
        // Transform 업데이트
        UpdateWorldTransform();
    }
}

void USceneComponent::DetachFromComponent()
{
    if (AttachParent)
    {
        AttachParent->RemoveChild(this);
        AttachParent = nullptr;
        
        // World Transform을 유지하도록 Relative Transform 업데이트
        RelativeLocation = WorldLocation;
        RelativeRotation = WorldRotation;
        RelativeScale = WorldScale;
    }
}

void USceneComponent::UpdateWorldTransform()
{
    if (AttachParent)
    {
        // 부모가 있으면 부모 기준으로 World Transform 계산
        if (!bAbsoluteLocation)
        {
            WorldLocation = AttachParent->GetComponentLocation() + RelativeLocation;
        }
        
        if (!bAbsoluteRotation)
        {
            WorldRotation = AttachParent->GetComponentRotation() + RelativeRotation;
        }
        
        if (!bAbsoluteScale)
        {
            FVector ParentScale = AttachParent->GetComponentScale();
            WorldScale = FVector(
                RelativeScale.X * ParentScale.X,
                RelativeScale.Y * ParentScale.Y,
                RelativeScale.Z * ParentScale.Z
            );
        }
    }
    else
    {
        // 부모가 없으면 Relative = World
        WorldLocation = RelativeLocation;
        WorldRotation = RelativeRotation;
        WorldScale = RelativeScale;
    }
    
    UpdateChildTransforms();
}

void USceneComponent::UpdateChildTransforms()
{
    // 모든 자식의 Transform 업데이트
    for (USceneComponent* Child : AttachChildren)
    {
        if (Child)
        {
            Child->UpdateWorldTransform();
        }
    }
}

void USceneComponent::AddChild(USceneComponent* Child)
{
    if (Child && std::find(AttachChildren.begin(), AttachChildren.end(), Child) == AttachChildren.end())
    {
        AttachChildren.push_back(Child);
    }
}

void USceneComponent::RemoveChild(USceneComponent* Child)
{
    auto it = std::find(AttachChildren.begin(), AttachChildren.end(), Child);
    if (it != AttachChildren.end())
    {
        AttachChildren.erase(it);
    }
}