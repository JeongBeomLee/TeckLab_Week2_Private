#pragma once
#include "Object.h"
#include "Containers.h"
#include "Actor.h"

// 전방 선언
class UWorld;

// 레벨 클래스 - 액터들의 컨테이너
class ULevel : public UObject
{
    UCLASS()
    GENERATED_BODY(ULevel, UObject)

public:
    ULevel();
    virtual ~ULevel();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "ULevel"; }

    // 레벨 관리
    virtual void BeginPlay();
    virtual void EndPlay();
    virtual void Tick(float DeltaTime);

    // 액터 관리
    void AddActor(AActor* Actor);
    bool RemoveActor(AActor* Actor);
    void RemoveAllActors();

    // 액터 검색
    AActor* FindActor(const FName& ActorName) const;
    template<typename T>
    T* FindActor(const FName& ActorName) const;

    template<typename T>
    TArray<T*> GetActorsOfClass() const;

    // 액터 접근
    const TArray<AActor*>& GetActors() const { return Actors; }
    int32 GetNumActors() const { return static_cast<int32>(Actors.size()); }

    // 레벨 상태
    bool IsLoaded() const { return bIsLoaded; }
    bool IsVisible() const { return bIsVisible; }
    bool IsCurrent() const { return bIsCurrent; }

    void SetLoaded(bool bLoaded) { bIsLoaded = bLoaded; }
    void SetVisible(bool bVisible) { bIsVisible = bVisible; }
    void SetCurrent(bool bCurrent) { bIsCurrent = bCurrent; }

    // 월드 참조
    UWorld* GetWorld() const { return OwningWorld; }
    void SetWorld(UWorld* World) { OwningWorld = World; }

    // 레벨 정보
    const FName& GetLevelName() const { return LevelName; }
    void SetLevelName(const FName& Name) { LevelName = Name; }

    // 레벨 로드/언로드
    virtual void LoadLevel();
    virtual void UnloadLevel();

    // 레벨 초기화
    virtual void InitializeLevel();
    virtual void CleanupLevel();

    // 액터 스폰
    template<typename T>
    T* SpawnActor(const FVector& Location = FVector::Zero, const FVector& Rotation = FVector::Zero, const FName& ActorName = FName(""));

    // 액터 제거
    bool DestroyActor(AActor* Actor);

    // 레벨 통계
    struct FLevelStats
    {
        int32 TotalActors;
        int32 StaticMeshActors;
        int32 LightActors;
        int32 OtherActors;
        size_t MemoryUsage;

        FLevelStats() : TotalActors(0), StaticMeshActors(0), LightActors(0), OtherActors(0), MemoryUsage(0) {}
    };

    FLevelStats GetLevelStats() const;

protected:
    // 액터 컨테이너
    TArray<AActor*> Actors;

    // 레벨 상태
    bool bIsLoaded;
    bool bIsVisible;
    bool bIsCurrent;

    // 레벨 정보
    FName LevelName;
    UWorld* OwningWorld;

    // 내부 헬퍼 함수들
    virtual void NotifyActorAdded(AActor* Actor);
    virtual void NotifyActorRemoved(AActor* Actor);

    // 액터 등록/해제
    void RegisterActor(AActor* Actor);
    void UnregisterActor(AActor* Actor);

private:
    // 액터 관리 헬퍼
    void ValidateActors();
    void CleanupNullActors();
};

// 템플릿 구현
template<typename T>
T* ULevel::FindActor(const FName& ActorName) const
{
    for (AActor* Actor : Actors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            T* CastedActor = Cast<T>(Actor);
            if (CastedActor)
            {
                return CastedActor;
            }
        }
    }
    return nullptr;
}

template<typename T>
TArray<T*> ULevel::GetActorsOfClass() const
{
    TArray<T*> Result;

    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            T* CastedActor = Cast<T>(Actor);
            if (CastedActor)
            {
                Result.push_back(CastedActor);
            }
        }
    }

    return Result;
}

template<typename T>
T* ULevel::SpawnActor(const FVector& Location, const FVector& Rotation, const FName& ActorName)
{
    T* NewActor = NewObject<T>(this, ActorName);
    if (NewActor)
    {
        NewActor->SetActorLocation(Location);
        NewActor->SetActorRotation(Rotation);
        AddActor(NewActor);
        return NewActor;
    }
    return nullptr;
}