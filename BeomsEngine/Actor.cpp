#include "pch.h"
#include "Actor.h"
#include "ActorComponent.h"

AActor::AActor()
    : UObject()
    , Location(FVector::Zero)
    , Rotation(FVector::Zero)
    , Scale(FVector::One)
    , bHidden(false)
    , bCanEverTick(true)
    , bActorEnableCollision(true)
    , bBlockInput(false)
    , RootComponent(nullptr)
    , World(nullptr)
    , Owner(nullptr)
{
    SetName(TEXT("Actor"));
}

AActor::~AActor()
{
    // 컴포넌트들 정리
    for (UActorComponent* Component : Components)
    {
        if (Component && !Component->IsPendingKill())
        {
            Component->MarkPendingKill();
            delete Component;
        }
    }
    Components.clear();
    
    // 소유된 액터들 정리
    for (AActor* OwnedActor : OwnedActors)
    {
        if (OwnedActor && !OwnedActor->IsPendingKill())
        {
            OwnedActor->SetOwner(nullptr);
        }
    }
    OwnedActors.clear();
}

void AActor::BeginPlay()
{
    UObject::BeginPlay();
    
    // 모든 컴포넌트의 BeginPlay 호출
    for (UActorComponent* Component : Components)
    {
        if (Component && Component->IsValid())
        {
            Component->BeginPlay();
        }
    }
}

void AActor::EndPlay()
{
    // 모든 컴포넌트의 EndPlay 호출
    for (UActorComponent* Component : Components)
    {
        if (Component && Component->IsValid())
        {
            Component->EndPlay();
        }
    }
    
    UObject::EndPlay();
}

void AActor::Tick(float DeltaTime)
{
    UObject::Tick(DeltaTime);
    
    if (!bCanEverTick)
        return;
    
    // 모든 컴포넌트의 Tick 호출
    for (UActorComponent* Component : Components)
    {
        if (Component && Component->IsValid() && Component->CanTick())
        {
            Component->Tick(DeltaTime);
        }
    }
}

void AActor::SetActorLocation(const FVector& NewLocation)
{
    Location = NewLocation;
    UpdateTransform();
}

void AActor::SetActorRotation(const FVector& NewRotation)
{
    Rotation = NewRotation;
    UpdateTransform();
}

void AActor::SetActorScale(const FVector& NewScale)
{
    Scale = NewScale;
    UpdateTransform();
}

FMatrix AActor::GetActorTransform() const
{
    // 스케일 -> 회전 -> 평행이동 순서로 변환 행렬 생성
    FMatrix ScaleMatrix = FMatrix::CreateScale(Scale);
    
    // 회전을 라디안으로 변환 (Rotation은 degree)
    float PitchRad = Rotation.X * (3.14159265359f / 180.0f);
    float YawRad = Rotation.Y * (3.14159265359f / 180.0f);
    float RollRad = Rotation.Z * (3.14159265359f / 180.0f);
    
    FMatrix RotationMatrix = FMatrix::CreateRotationFromEuler(RollRad, PitchRad, YawRad);
    FMatrix TranslationMatrix = FMatrix::CreateTranslation(Location);
    
    return ScaleMatrix * RotationMatrix * TranslationMatrix;
}

void AActor::SetActorTransform(const FMatrix& NewTransform)
{
    // 행렬에서 위치, 회전, 스케일 추출하는 간단한 구현
    // 실제 구현에서는 더 정교한 행렬 분해가 필요
    Location = FVector(NewTransform[0][3], NewTransform[1][3], NewTransform[2][3]);
    UpdateTransform();
}

UActorComponent* AActor::FindComponentByName(const FString& ComponentName) const
{
    for (UActorComponent* Component : Components)
    {
        if (Component && Component->GetName() == ComponentName)
        {
            return Component;
        }
    }
    return nullptr;
}

void AActor::AddComponent(UActorComponent* Component)
{
    if (Component && Component->IsValid())
    {
        Components.push_back(Component);
        Component->SetOuter(this);
    }
}

void AActor::RemoveComponent(UActorComponent* Component)
{
    if (Component)
    {
        auto it = std::find(Components.begin(), Components.end(), Component);
        if (it != Components.end())
        {
            Components.erase(it);
            Component->SetOuter(nullptr);
        }
    }
}

void AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
    RootComponent = NewRootComponent;
    if (RootComponent)
    {
        AddComponent(RootComponent);
    }
}

void AActor::GetOwnedActors(TArray<AActor*>& OutActors) const
{
    OutActors = OwnedActors;
}

void AActor::AttachToActor(AActor* ParentActor)
{
    if (ParentActor && ParentActor != this)
    {
        if (Owner)
        {
            DetachFromActor();
        }
        
        Owner = ParentActor;
        ParentActor->OwnedActors.push_back(this);
    }
}

void AActor::DetachFromActor()
{
    if (Owner)
    {
        auto it = std::find(Owner->OwnedActors.begin(), Owner->OwnedActors.end(), this);
        if (it != Owner->OwnedActors.end())
        {
            Owner->OwnedActors.erase(it);
        }
        Owner = nullptr;
    }
}

void AActor::UpdateTransform()
{
    // Transform이 변경되었을 때의 처리
    // 실제로는 렌더링 시스템, 물리 시스템 등에 알림
}

// 템플릿 함수들의 구현은 헤더에서 해야 함
template<typename T>
T* AActor::CreateComponent(const FString& ComponentName)
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must derive from UActorComponent");
    
    T* NewComponent = new T();
    if (NewComponent)
    {
        if (!ComponentName.empty())
        {
            NewComponent->SetName(ComponentName);
        }
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
        T* TypedComponent = dynamic_cast<T*>(Component);
        if (TypedComponent)
        {
            return TypedComponent;
        }
    }
    return nullptr;
}