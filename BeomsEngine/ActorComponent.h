#pragma once
#include "ObjectMacros.h"
#include "Vector.h"

// 전방 선언
class AActor;

// UActorComponent 기본 클래스
class UActorComponent : public UObject
{
	UCLASS()
    GENERATED_BODY(UActorComponent, UObject)
public:
    UActorComponent();
    virtual ~UActorComponent();

    // UObject 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override; 
    virtual void Tick(float DeltaTime) override;
    virtual FString GetClassName() const override { return "UActorComponent"; }

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

template<typename T>
T* UActorComponent::GetOwner() const
{
    return Cast<T>(GetOuter());
}