#pragma once

// 전방 선언
class UActorComponent;
class USceneComponent;

// AActor 클래스
class AActor : public UObject
{
    UCLASS()
    GENERATED_BODY(AActor, UObject)
public:
    AActor();
    virtual ~AActor();

    // UObject 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual FString GetClassName() const override { return "AActor"; }

    // Transform 관련 - RootComponent에 위임
    FVector GetActorLocation() const;
    FVector GetActorRotation() const;
    FVector GetActorScale() const;

    void SetActorLocation(const FVector& NewLocation);
    void SetActorRotation(const FVector& NewRotation);
    void SetActorScale(const FVector& NewScale);

    FMatrix GetActorTransform() const;
    void SetActorTransform(const FMatrix& NewTransform);

    // 액터 상태
    bool IsHidden() const { return bHidden; }
    void SetHidden(bool bNewHidden) { bHidden = bNewHidden; }

    bool CanTick() const { return bCanEverTick; }
    void SetCanEverTick(bool bNewCanTick) { bCanEverTick = bNewCanTick; }

    // 컴포넌트 관리
    template<typename T>
    T* CreateComponent(const FName& ComponentName = FName());

    template<typename T>
    T* FindComponentByClass() const;

    UActorComponent* FindComponentByName(const FName& ComponentName) const;
    void AddComponent(UActorComponent* Component);
    void RemoveComponent(UActorComponent* Component);

    const TArray<UActorComponent*>& GetComponents() const { return Components; }

    // 루트 컴포넌트
    USceneComponent* GetRootComponent() const { return RootComponent; }
    void SetRootComponent(USceneComponent* NewRootComponent);

    // 월드 상호작용
    class UWorld* GetWorld() const { return World; }
    void SetWorld(class UWorld* NewWorld) { World = NewWorld; }

    // 액터 관계
    AActor* GetOwner() const { return Owner; }
    void SetOwner(AActor* NewOwner) { Owner = NewOwner; }

    void GetOwnedActors(TArray<AActor*>& OutActors) const;
    void AttachToActor(AActor* ParentActor);
    void DetachFromActor();

protected:
    // 액터 상태
    bool bHidden;
    bool bCanEverTick;
    bool bActorEnableCollision;
    bool bBlockInput;

    // 컴포넌트들
    TArray<UActorComponent*> Components;
    USceneComponent* RootComponent;

    // 월드 참조
    class UWorld* World;

    // 소유권 관계
    AActor* Owner;
    TArray<AActor*> OwnedActors;

    // 액터 이벤트
    virtual void NotifyActorOnClicked() {}
    virtual void NotifyHit(AActor* OtherActor, FVector HitLocation) {}

private:
    // 내부 헬퍼 함수들
};

template<typename T>
T* AActor::CreateComponent(const FName& ComponentName)
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must derive from UActorComponent");

    FName ActualComponentName = ComponentName.IsNone() ?
        FName(FString("Default") + T::GetStaticClass()->GetNameString()) : ComponentName;

    T* NewComponent = NewObject<T>(this, ActualComponentName);
    if (NewComponent)
    {
        AddComponent(NewComponent);
    }
    return NewComponent;
}

template<typename T>
T* AActor::FindComponentByClass() const
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must derive from UActorComponent");

    for (UActorComponent* Component : Components)
    {
		T* TypedComponent = Cast<T>(Component);
        if (TypedComponent)
        {
            return TypedComponent;
        }
    }
    return nullptr;
}