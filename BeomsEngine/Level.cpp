#include "pch.h"
#include "Level.h"
#include "StaticMeshActor.h"
#include "World.h"
#include "ObjectInitializer.h"

IMPLEMENT_CLASS(ULevel, UObject)

ULevel::ULevel()
    : bIsLoaded(false)
    , bIsVisible(true)
    , bIsCurrent(false)
    , LevelName(FName("DefaultLevel"))
    , OwningWorld(nullptr)
{
}

ULevel::~ULevel()
{
    CleanupLevel();
}

void ULevel::BeginPlay()
{
    if (!bIsLoaded)
    {
        return;
    }

    // 모든 액터들의 BeginPlay 호출
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            Actor->BeginPlay();
        }
    }
}

void ULevel::EndPlay()
{
    // 모든 액터들의 EndPlay 호출
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            Actor->EndPlay();
        }
    }
}

void ULevel::Tick(float DeltaTime)
{
    if (!bIsLoaded || !bIsVisible)
    {
        return;
    }

    // 유효하지 않은 액터들 정리
    CleanupNullActors();

    // 모든 액터들의 Tick 호출
    for (AActor* Actor : Actors)
    {
        if (Actor && Actor->CanEverTick())
        {
            Actor->Tick(DeltaTime);
        }
    }
}

void ULevel::AddActor(AActor* Actor)
{
    if (!Actor)
    {
        return;
    }

    // 이미 추가된 액터인지 확인
    auto It = std::find(Actors.begin(), Actors.end(), Actor);
    if (It != Actors.end())
    {
        return;
    }

    // 액터 추가
    Actors.push_back(Actor);
    RegisterActor(Actor);
    NotifyActorAdded(Actor);
}

bool ULevel::RemoveActor(AActor* Actor)
{
    if (!Actor)
    {
        return false;
    }

    auto It = std::find(Actors.begin(), Actors.end(), Actor);
    if (It != Actors.end())
    {
        Actor->MarkPendingKill();
        UnregisterActor(Actor);
        NotifyActorRemoved(Actor);
        Actors.erase(It);
        return true;
    }

    return false;
}

void ULevel::RemoveAllActors()
{
    // 모든 액터들을 역순으로 제거
    for (auto It = Actors.rbegin(); It != Actors.rend(); ++It)
    {
        AActor* Actor = *It;
        if (Actor)
        {
            Actor->MarkPendingKill();
            UnregisterActor(Actor);
            NotifyActorRemoved(Actor);
        }
    }

    Actors.clear();
}

AActor* ULevel::FindActor(const FName& ActorName) const
{
    for (AActor* Actor : Actors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            return Actor;
        }
    }
    return nullptr;
}

void ULevel::LoadLevel()
{
    if (bIsLoaded)
    {
        return;
    }

    InitializeLevel();
    bIsLoaded = true;

    // 레벨이 로드되면 모든 액터들 초기화
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            // 액터 초기화 로직 필요시 추가
        }
    }
}

void ULevel::UnloadLevel()
{
    if (!bIsLoaded)
    {
        return;
    }

    EndPlay();
    CleanupLevel();
    bIsLoaded = false;
}

void ULevel::InitializeLevel()
{
    // 레벨 초기화 로직
    LevelName = FName("Level_" + GetUniqueID());
}

void ULevel::CleanupLevel()
{
    EndPlay();
    RemoveAllActors();
}

bool ULevel::DestroyActor(AActor* Actor)
{
    if (!Actor)
    {
        return false;
    }

    // 액터 제거
    bool bRemoved = RemoveActor(Actor);

    if (bRemoved)
    {
        // 액터 EndPlay 호출
        Actor->EndPlay();
        return true;
    }

    return false;
}

ULevel::FLevelStats ULevel::GetLevelStats() const
{
    FLevelStats Stats;

    Stats.TotalActors = static_cast<int32>(Actors.size());

    for (AActor* Actor : Actors)
    {
        if (!Actor)
        {
            continue;
        }

        // 액터 타입별 카운트
        if (Cast<AStaticMeshActor>(Actor))
        {
            Stats.StaticMeshActors++;
        }
        else
        {
            Stats.OtherActors++;
        }
    }

    // 메모리 사용량 계산 (근사치)
    Stats.MemoryUsage = Stats.TotalActors * sizeof(AActor*);
    Stats.MemoryUsage += sizeof(ULevel);

    return Stats;
}

void ULevel::NotifyActorAdded(AActor* Actor)
{
    if (Actor && OwningWorld)
    {
        // 월드에 액터 추가 알림
        Actor->SetLevel(this);
    }
}

void ULevel::NotifyActorRemoved(AActor* Actor)
{
    if (Actor)
    {
        // 액터에서 레벨 참조 제거
        Actor->SetLevel(nullptr);
    }
}

void ULevel::RegisterActor(AActor* Actor)
{
    if (Actor)
    {
        // 액터에 레벨 설정
        Actor->SetLevel(this);

        // 월드 설정
        if (OwningWorld)
        {
            Actor->SetWorld(OwningWorld);
        }
    }
}

void ULevel::UnregisterActor(AActor* Actor)
{
    if (Actor)
    {
        Actor->SetLevel(nullptr);
        Actor->SetWorld(nullptr);
    }
}

void ULevel::ValidateActors()
{
    // 유효하지 않은 액터들 찾기
    for (size_t i = 0; i < Actors.size(); )
    {
        if (!Actors[i])
        {
            Actors.erase(Actors.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void ULevel::CleanupNullActors()
{
    // nullptr인 액터들 제거
    Actors.erase(
        std::remove(Actors.begin(), Actors.end(), nullptr),
        Actors.end()
    );
}