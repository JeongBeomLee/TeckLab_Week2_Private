#include "pch.h"
#include "ActorComponent.h"
#include "Actor.h"
#include "ObjectInitializer.h"

IMPLEMENT_CLASS(UActorComponent, UObject)

UActorComponent::UActorComponent()
    : UObject()
    , bIsActive(true)
    , bCanEverTick(false)
    , bWantsInitializeComponent(true)
    , bHasBeenInitialized(false)
    , bRegistered(false)
    , bAutoActivate(true)
{
    SetName(FName("ActorComponent"));
}

UActorComponent::~UActorComponent()
{
    if (bRegistered)
    {
        UnregisterComponent();
    }
}

void UActorComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (!bHasBeenInitialized && bWantsInitializeComponent)
    {
        InitializeComponent();
        bHasBeenInitialized = true;
    }
    
    if (!bRegistered)
    {
        RegisterComponent();
    }
    
    if (bAutoActivate && !bIsActive)
    {
        SetActive(true);
    }
}

void UActorComponent::EndPlay()
{
    if (bIsActive)
    {
        SetActive(false);
    }
    
    if (bRegistered)
    {
        UnregisterComponent();
    }
    
    if (bHasBeenInitialized)
    {
        UninitializeComponent();
        bHasBeenInitialized = false;
    }
    
    Super::EndPlay();
}

void UActorComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UActorComponent::SetActive(bool bNewActive)
{
    if (bIsActive != bNewActive)
    {
        bIsActive = bNewActive;
        
        if (bIsActive)
        {
            OnActivated();
        }
        else
        {
            OnDeactivated();
        }
    }
}

AActor* UActorComponent::GetOwner() const
{
    return Cast<AActor>(GetOuter());
}

class UWorld* UActorComponent::GetWorld() const
{
    AActor* Owner = GetOwner();
    return Owner ? Owner->GetWorld() : nullptr;
}

void UActorComponent::RegisterComponent()
{
    if (!bRegistered)
    {
        bRegistered = true;
        OnRegister();
    }
}

void UActorComponent::UnregisterComponent()
{
    if (bRegistered)
    {
        OnUnregister();
        bRegistered = false;
    }
}

