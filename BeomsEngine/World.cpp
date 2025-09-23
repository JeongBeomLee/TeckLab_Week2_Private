#include "pch.h"
#include "World.h"
#include "Actor.h"
#include "StaticMeshActor.h"
#include "ObjectInitializer.h"

IMPLEMENT_CLASS(UWorld, UObject)

// 정적 멤버 정의
UWorld* UWorld::CurrentWorldInstance = nullptr;

UWorld::UWorld()
    : CurrentLevel(nullptr)
    , bIsInitialized(false)
    , bIsPlaying(false)
    , bIsPaused(false)
    , WorldName(FName("DefaultWorld"))
    , TimeSeconds(0.0f)
    , RealTimeSeconds(0.0f)
    , DeltaTimeSeconds(0.0f)
{
}

UWorld::~UWorld()
{
    CleanupWorld();

    // 현재 월드 인스턴스 정리
    if (CurrentWorldInstance == this)
    {
        CurrentWorldInstance = nullptr;
    }
}

void UWorld::BeginPlay()
{
    if (!bIsInitialized)
    {
        InitializeWorld();
    }

    bIsPlaying = true;

    // 현재 레벨의 BeginPlay 호출
    if (CurrentLevel && CurrentLevel->IsLoaded())
    {
        CurrentLevel->BeginPlay();
    }
}

void UWorld::EndPlay()
{
    bIsPlaying = false;

    // 현재 레벨의 EndPlay 호출
    if (CurrentLevel)
    {
        CurrentLevel->EndPlay();
    }
}

void UWorld::Tick(float DeltaTime)
{
    if (!bIsPlaying || bIsPaused)
    {
        return;
    }

    // 시간 관리 업데이트
    AddDeltaTime(DeltaTime);

    // 현재 레벨 업데이트
    if (CurrentLevel && CurrentLevel->IsLoaded())
    {
        CurrentLevel->Tick(DeltaTime);
    }
}

void UWorld::SetCurrentLevel(ULevel* Level)
{
    if (CurrentLevel)
    {
        CurrentLevel->SetCurrent(false);
        CurrentLevel->SetWorld(nullptr);
        CurrentLevel->MarkPendingKill();
    }

    CurrentLevel = Level;

    if (CurrentLevel)
    {
        CurrentLevel->SetCurrent(true);
        CurrentLevel->SetWorld(this);
    }
}

void UWorld::InitializeWorld()
{
    if (bIsInitialized)
    {
        return;
    }

    // 기본 레벨 생성
    if (!CurrentLevel)
    {
        CreateDefaultLevel();
    }

    // 시간 초기화
    TimeSeconds = 0.0f;
    RealTimeSeconds = 0.0f;
    DeltaTimeSeconds = 0.0f;

    bIsInitialized = true;
}

void UWorld::CleanupWorld()
{
    EndPlay();
    SetCurrentLevel(nullptr);
    bIsInitialized = false;
}

AActor* UWorld::FindActor(const FName& ActorName) const
{
    if (CurrentLevel)
    {
        return CurrentLevel->FindActor(ActorName);
    }
    return nullptr;
}

TArray<AActor*> UWorld::GetAllActors() const
{
    if (CurrentLevel)
    {
        return CurrentLevel->GetActors();
    }
    return TArray<AActor*>();
}

int32 UWorld::GetTotalActorCount() const
{
    if (CurrentLevel)
    {
        return CurrentLevel->GetNumActors();
    }
    return 0;
}

int32 UWorld::GetActorCount() const
{
    return GetTotalActorCount();
}

void UWorld::AddDeltaTime(float DeltaTime)
{
    if (!bIsPaused)
    {
        TimeSeconds += DeltaTime;
    }
    RealTimeSeconds += DeltaTime;
    DeltaTimeSeconds = DeltaTime;
}

ULevel* UWorld::CreateLevel(const FName& LevelName)
{
    ULevel* NewLevel = NewObject<ULevel>(this, LevelName);
    if (NewLevel)
    {
        NewLevel->SetLevelName(LevelName);
        return NewLevel;
    }
    return nullptr;
}

void UWorld::CreateDefaultLevel()
{
    ULevel* DefaultLevel = CreateLevel(FName("DefaultLevel"));
    if (DefaultLevel)
    {
        SetCurrentLevel(DefaultLevel);
        DefaultLevel->LoadLevel();
    }
}

UWorld* UWorld::CreateWorld(const FName& WorldName)
{
    UWorld* NewWorld = NewObject<UWorld>(nullptr, WorldName);
    if (NewWorld)
    {
        NewWorld->SetWorldName(WorldName);
        NewWorld->InitializeWorld();

        // 첫 번째 월드를 현재 월드로 설정
        if (!CurrentWorldInstance)
        {
            SetCurrentWorld(NewWorld);
        }
        else
        {
            CurrentWorldInstance->MarkPendingKill();
            SetCurrentWorld(NewWorld);
        }
    }
    return NewWorld;
}

void UWorld::DestroyWorld(UWorld* World)
{
    if (World)
    {
        World->CleanupWorld();

        if (CurrentWorldInstance == World)
        {
            CurrentWorldInstance->MarkPendingKill();
            CurrentWorldInstance = nullptr;
        }

        // GC가 메모리 정리 처리
    }
}

// 전역 함수 구현
UWorld* GetWorld()
{
    return UWorld::GetCurrentWorld();
}

void SetGlobalWorld(UWorld* World)
{
    UWorld::SetCurrentWorld(World);
}