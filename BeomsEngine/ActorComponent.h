#pragma once
#include "Object.h"
#include "Vector.h"

// 전방 선언
class AActor;

// Unreal Engine UActorComponent 기본 클래스
class UActorComponent : public UObject
{
public:
    UActorComponent();
    virtual ~UActorComponent();

    // UObject 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override; 
    virtual void Tick(float DeltaTime) override;
    virtual FString GetClassName() const override { return TEXT("UActorComponent"); }

    // 컴포넌트 상태
    bool IsActive() const { return bIsActive; }
    void SetActive(bool bNewActive);
    
    bool CanTick() const { return bCanEverTick; }
    void SetCanEverTick(bool bNewCanTick) { bCanEverTick = bNewCanTick; }
    
    bool IsRegistered() const { return bRegistered; }
    
    // 소유자 액터
    AActor* GetOwner() const;
    
    template<typename T>
    T* GetOwner() const;
    
    // 컴포넌트 이벤트
    virtual void OnRegister() {}
    virtual void OnUnregister() {}
    virtual void OnActivated() {}
    virtual void OnDeactivated() {}
    
    // 컴포넌트 초기화
    virtual void InitializeComponent() {}
    virtual void UninitializeComponent() {}
    
    // 월드 상호작용
    class UWorld* GetWorld() const;
    
protected:
    // 컴포넌트 상태
    bool bIsActive;
    bool bCanEverTick;
    bool bWantsInitializeComponent;
    bool bHasBeenInitialized;
    bool bRegistered;
    
    // 자동 활성화 설정
    bool bAutoActivate;
    
    // 컴포넌트 태그들
    TArray<FString> ComponentTags;
    
    // 내부 함수들
    void RegisterComponent();
    void UnregisterComponent();
    
private:
    friend class AActor;
};

// Scene Component - 3D 공간에서의 위치를 가지는 컴포넌트
class USceneComponent : public UActorComponent
{
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