#pragma once
#include "Object.h"
#include "Containers.h"
#include "Math.h"
#include "Level.h"

// 전방 선언
class AActor;

// 월드 클래스 - 레벨들의 컨테이너이자 게임 월드의 최상위 관리자
class UWorld : public UObject
{
    UCLASS()
    GENERATED_BODY(UWorld, UObject)

public:
    UWorld();
    virtual ~UWorld();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "UWorld"; }

    // 월드 관리
    virtual void BeginPlay();
    virtual void EndPlay();
    virtual void Tick(float DeltaTime);

    // 현재 레벨 관리
    ULevel* GetCurrentLevel() const { return CurrentLevel; }
    void SetCurrentLevel(ULevel* Level);

    // 레벨 생성 헬퍼
    ULevel* CreateLevel(const FName& LevelName = FName("DefaultLevel"));

    // 월드 상태
    bool IsInitialized() const { return bIsInitialized; }
    bool IsPlaying() const { return bIsPlaying; }
    bool IsPaused() const { return bIsPaused; }

    void SetPlaying(bool bPlaying) { bIsPlaying = bPlaying; }
    void SetPaused(bool bPaused) { bIsPaused = bPaused; }

    // 월드 초기화/정리
    virtual void InitializeWorld();
    virtual void CleanupWorld();

    // 액터 관리 (현재 레벨의 액터들)
    AActor* FindActor(const FName& ActorName) const;

    template<typename T>
    TArray<T*> GetActorsOfClass() const;

    TArray<AActor*> GetAllActors() const;
    int32 GetTotalActorCount() const;

    // 월드 설정
    const FName& GetWorldName() const { return WorldName; }
    void SetWorldName(const FName& Name) { WorldName = Name; }

    // 시간 관리
    float GetTimeSeconds() const { return TimeSeconds; }
    float GetRealTimeSeconds() const { return RealTimeSeconds; }
    float GetDeltaTimeSeconds() const { return DeltaTimeSeconds; }

    void SetTimeSeconds(float Time) { TimeSeconds = Time; }
    void AddDeltaTime(float DeltaTime);

    // 월드 경계
    struct FWorldBounds
    {
        FVector Min;
        FVector Max;
        FVector Center;
        FVector Extent;

        FWorldBounds() : Min(FVector(-10000.0f)), Max(FVector(10000.0f)), Center(FVector::Zero), Extent(FVector(10000.0f)) {}
    };

    const FWorldBounds& GetWorldBounds() const { return WorldBounds; }
    void SetWorldBounds(const FWorldBounds& Bounds) { WorldBounds = Bounds; }

    // 기본 월드 정보
    int32 GetActorCount() const;

    // 싱글톤 접근 (현재 활성 월드)
    static UWorld* GetCurrentWorld() { return CurrentWorldInstance; }
    static void SetCurrentWorld(UWorld* World) { CurrentWorldInstance = World; }

    // 월드 생성/제거
    static UWorld* CreateWorld(const FName& WorldName = FName("DefaultWorld"));
    static void DestroyWorld(UWorld* World);

    // 기본 레벨 생성
    void CreateDefaultLevel();

    // 월드 저장/로드 (추후 구현용)
    virtual bool SaveWorld(const FString& FilePath) { return false; }
    virtual bool LoadWorld(const FString& FilePath) { return false; }

protected:
    // 현재 활성 레벨
    ULevel* CurrentLevel;

    // 월드 상태
    bool bIsInitialized;
    bool bIsPlaying;
    bool bIsPaused;

    // 월드 정보
    FName WorldName;

    // 시간 관리
    float TimeSeconds;
    float RealTimeSeconds;
    float DeltaTimeSeconds;

    // 월드 경계
    FWorldBounds WorldBounds;

    // 정적 인스턴스 (현재 월드)
    static UWorld* CurrentWorldInstance;
};

// 템플릿 구현
template<typename T>
TArray<T*> UWorld::GetActorsOfClass() const
{
    if (CurrentLevel)
    {
        return CurrentLevel->GetActorsOfClass<T>();
    }
    return TArray<T*>();
}

// 전역 월드 접근 함수
UWorld* GetWorld();
void SetGlobalWorld(UWorld* World);