#pragma once
#include "ActorComponent.h"
#include "Vector.h"
#include "Array.h"

// Scene Component - 3D 공간에서의 위치를 가지는 컴포넌트
class USceneComponent : public UActorComponent
{
    UCLASS()
    GENERATED_BODY(USceneComponent, UActorComponent)
public:
    USceneComponent();
    virtual ~USceneComponent();
    
    virtual FString GetClassName() const override { return TEXT("USceneComponent"); }
    
    // Transform 관련
    FVector GetComponentLocation() const { return WorldLocation; }
    FVector GetComponentRotation() const { return WorldRotation; }
    FVector GetComponentScale() const { return WorldScale; }
    
    void SetWorldLocation(const FVector& NewLocation);
    void SetWorldRotation(const FVector& NewRotation);
    void SetWorldScale(const FVector& NewScale);
    
    // 상대 Transform
    FVector GetRelativeLocation() const { return RelativeLocation; }
    FVector GetRelativeRotation() const { return RelativeRotation; }
    FVector GetRelativeScale() const { return RelativeScale; }
    
    void SetRelativeLocation(const FVector& NewLocation);
    void SetRelativeRotation(const FVector& NewRotation);
    void SetRelativeScale(const FVector& NewScale);
    
    // 컴포넌트 계층구조
    USceneComponent* GetAttachParent() const { return AttachParent; }
    void AttachToComponent(USceneComponent* Parent);
    void DetachFromComponent();
    
    const TArray<USceneComponent*>& GetAttachChildren() const { return AttachChildren; }
    
    // 가시성
    bool IsVisible() const { return bVisible; }
    void SetVisibility(bool bNewVisible) { bVisible = bNewVisible; }
    
protected:
    // World Transform
    FVector WorldLocation;
    FVector WorldRotation;
    FVector WorldScale;
    
    // Relative Transform (부모 기준)
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale;
    
    // 컴포넌트 계층구조
    USceneComponent* AttachParent;
    TArray<USceneComponent*> AttachChildren;
    
    // 상태
    bool bVisible;
    bool bAbsoluteLocation;
    bool bAbsoluteRotation;
    bool bAbsoluteScale;
    
    // Transform 업데이트
    virtual void UpdateWorldTransform();
    virtual void UpdateChildTransforms();
    
private:
    void AddChild(USceneComponent* Child);
    void RemoveChild(USceneComponent* Child);
};