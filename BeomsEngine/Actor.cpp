#include "pch.h"
#include "Actor.h"
#include "SceneComponent.h"
#include "Math.h"
#include "ObjectInitializer.h"

IMPLEMENT_CLASS(AActor, UObject)

AActor::AActor()
    : UObject()
    , bHidden(false)
    , bCanEverTick(true)
    , bActorEnableCollision(true)
    , bBlockInput(false)
    , RootComponent(nullptr)
    , World(nullptr)
    , Owner(nullptr)
{
    SetName(TEXT("Actor"));

    // 기본 RootComponent 생성
    RootComponent = new USceneComponent();
    if (RootComponent)
    {
        RootComponent->SetName(TEXT("DefaultSceneRoot"));
        AddComponent(RootComponent);
    }
}

AActor::AActor(const FObjectInitializer& ObjectInitializer)
    : UObject(ObjectInitializer)
    , bHidden(false)
    , bCanEverTick(true)
    , bActorEnableCollision(true)
    , bBlockInput(false)
    , RootComponent(nullptr)
    , World(nullptr)
    , Owner(nullptr)
{
    if (GetName().empty())
    {
        SetName(TEXT("Actor"));
    }

    // 기본 RootComponent 생성
    RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    if (RootComponent)
    {
        AddComponent(RootComponent);
    }
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
	Super::BeginPlay();
    
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
    
    Super::EndPlay();
}

void AActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
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

// Transform 함수들 - RootComponent에 위임
FVector AActor::GetActorLocation() const
{
    if (RootComponent)
    {
		return RootComponent->GetComponentLocation();
    }

	return FVector::Zero;
}

FVector AActor::GetActorRotation() const
{
    if (RootComponent)
    {
        return RootComponent->GetComponentRotation();
    }

    return FVector::Zero;
}

FVector AActor::GetActorScale() const
{
    if (RootComponent)
    {
        return RootComponent->GetComponentScale();
    }

    return FVector::Zero;
}

void AActor::SetActorLocation(const FVector& NewLocation)
{
    if (RootComponent)
    {
        RootComponent->SetWorldLocation(NewLocation);
    }
}

void AActor::SetActorRotation(const FVector& NewRotation)
{
    if (RootComponent)
    {
        RootComponent->SetWorldRotation(NewRotation);
    }
}

void AActor::SetActorScale(const FVector& NewScale)
{
    if (RootComponent)
    {
        RootComponent->SetWorldScale(NewScale);
    }
}

FMatrix AActor::GetActorTransform() const
{
    if (!RootComponent)
        return FMatrix::Identity;
        
    // RootComponent의 Transform으로부터 행렬 생성
    FVector Location = RootComponent->GetComponentLocation();
    FVector Rotation = RootComponent->GetComponentRotation();
    FVector Scale = RootComponent->GetComponentScale();
    
    // Translation -> Rotation -> Scale 순서로 변환 행렬 생성 (T * R * S)
    FMatrix TranslationMatrix = FMatrix::CreateTranslation(Location);
    
    // 회전을 라디안으로 변환 (Rotation은 degree)
    float PitchRad = FMath::DegreesToRadians(Rotation.X);
    float YawRad = FMath::DegreesToRadians(Rotation.Y);
    float RollRad = FMath::DegreesToRadians(Rotation.Z);
    
    FMatrix RotationMatrix = FMatrix::CreateRotationFromEuler(RollRad, PitchRad, YawRad);
    FMatrix ScaleMatrix = FMatrix::CreateScale(Scale);
    
    return TranslationMatrix * RotationMatrix * ScaleMatrix;
}

void AActor::SetActorTransform(const FMatrix& NewTransform)
{
    if (!RootComponent)
        return;

    // 행렬 분해: Transform Matrix = Translation * Rotation * Scale (T * R * S)

    // 1. Translation 추출 (4번째 열)
    FVector Translation(NewTransform[0][3], NewTransform[1][3], NewTransform[2][3]);
    
    // 2. R*S 부분에서 Scale 추출 (3x3 좌상단 부분)
    FVector ScaleX(NewTransform[0][0], NewTransform[1][0], NewTransform[2][0]);
    FVector ScaleY(NewTransform[0][1], NewTransform[1][1], NewTransform[2][1]);
    FVector ScaleZ(NewTransform[0][2], NewTransform[1][2], NewTransform[2][2]);
    
    float ScaleXLength = ScaleX.Magnitude();
    float ScaleYLength = ScaleY.Magnitude();
    float ScaleZLength = ScaleZ.Magnitude();
    
    FVector Scale(ScaleXLength, ScaleYLength, ScaleZLength);
    
    // 3. R*S에서 Scale을 제거하여 Rotation 추출
    FMatrix RotationMatrix;
    RotationMatrix[0][0] = ScaleXLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[0][0] / ScaleXLength : 1.0f;
    RotationMatrix[1][0] = ScaleXLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[1][0] / ScaleXLength : 0.0f;
    RotationMatrix[2][0] = ScaleXLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[2][0] / ScaleXLength : 0.0f;
    
    RotationMatrix[0][1] = ScaleYLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[0][1] / ScaleYLength : 0.0f;
    RotationMatrix[1][1] = ScaleYLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[1][1] / ScaleYLength : 1.0f;
    RotationMatrix[2][1] = ScaleYLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[2][1] / ScaleYLength : 0.0f;
    
    RotationMatrix[0][2] = ScaleZLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[0][2] / ScaleZLength : 0.0f;
    RotationMatrix[1][2] = ScaleZLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[1][2] / ScaleZLength : 0.0f;
    RotationMatrix[2][2] = ScaleZLength > FMath::KINDA_SMALL_NUMBER ? NewTransform[2][2] / ScaleZLength : 1.0f;
    
    // 나머지 원소는 0 또는 1로 설정
    RotationMatrix[3][0] = RotationMatrix[3][1] = RotationMatrix[3][2] = 0.0f;
    RotationMatrix[0][3] = RotationMatrix[1][3] = RotationMatrix[2][3] = 0.0f;
    RotationMatrix[3][3] = 1.0f;
    
    // 회전 행렬에서 오일러 각 추출 (Z*X*Y 순서 - CreateRotationFromEuler와 일치)
    FVector Rotation;
    
    // Z*X*Y 분해: R = Rz(roll) * Rx(pitch) * Ry(yaw)
    float pitch = FMath::Asin(FMath::Clamp(RotationMatrix[2][1], -1.0f, 1.0f));
    
    if (FMath::Abs(FMath::Cos(pitch)) > FMath::KINDA_SMALL_NUMBER) 
    {
        // 일반적인 경우
        float roll = FMath::Atan2(-RotationMatrix[0][1], RotationMatrix[1][1]);
        float yaw = FMath::Atan2(-RotationMatrix[2][0], RotationMatrix[2][2]);
        
        Rotation.Z = FMath::RadiansToDegrees(roll);   // Roll
        Rotation.X = FMath::RadiansToDegrees(pitch);  // Pitch  
        Rotation.Y = FMath::RadiansToDegrees(yaw);    // Yaw
    } 
    else 
    {
        // Gimbal lock 상황 (pitch = 90도)
        float roll = FMath::Atan2(RotationMatrix[1][0], RotationMatrix[0][0]);
        
        Rotation.Z = FMath::RadiansToDegrees(roll);
        Rotation.X = FMath::RadiansToDegrees(pitch);
        Rotation.Y = 0.0f; // Yaw를 0으로 고정
    }
    
    // 추출된 Transform을 RootComponent에 적용
    RootComponent->SetWorldLocation(Translation);
    RootComponent->SetWorldRotation(Rotation);
    RootComponent->SetWorldScale(Scale);
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
