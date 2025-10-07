# Phase 3: Engine Core

## Overview
Engine Core는 게임 세계의 기본 구조를 정의하는 시스템입니다. World/Level 관리, Actor 시스템, Component 아키텍처로 구성되며, Unreal Engine의 구조를 모방합니다.

## System Architecture

```
Engine Core
├── World Management (게임 세계 관리)
│   ├── UWorld (최상위 게임 월드)
│   └── ULevel (액터 컨테이너)
├── Actor System (게임 오브젝트)
│   ├── AActor (기본 액터)
│   └── AStaticMeshActor (메시 액터)
└── Component Architecture (컴포지션 기반 설계)
    ├── UActorComponent (기본 컴포넌트)
    ├── USceneComponent (트랜스폼 컴포넌트)
    ├── UPrimitiveComponent (렌더링/물리 컴포넌트)
    ├── UMeshComponent (메시 컴포넌트)
    └── UStaticMeshComponent (정적 메시 컴포넌트)
```

---

## 1. World Management

### 1.1 UWorld (World.h:11)
**최상위 게임 월드 관리자 - Singleton + Composite Pattern**

#### 역할
- 레벨 컨테이너 관리
- 게임 시간 관리
- 전역 액터 접근
- 월드 상태 관리

#### 상속 관계
```
UObject
└── UWorld
```

#### 주요 멤버 변수
```cpp
// 레벨 관리
ULevel* CurrentLevel;           // 현재 활성 레벨

// 월드 상태
bool bIsInitialized;            // 초기화 여부
bool bIsPlaying;                // 플레이 중 여부
bool bIsPaused;                 // 일시정지 여부

// 월드 정보
FName WorldName;                // 월드 이름

// 시간 관리
float TimeSeconds;              // 게임 시간 (일시정지 영향 받음)
float RealTimeSeconds;          // 실제 시간 (일시정지 무관)
float DeltaTimeSeconds;         // 프레임 간격

// 월드 경계
FWorldBounds WorldBounds;       // 월드 공간 범위

// 싱글톤
static UWorld* CurrentWorldInstance;  // 현재 활성 월드
```

#### FWorldBounds (World.h:69)
**월드 공간 경계**

```cpp
struct FWorldBounds
{
    FVector Min;                // 최소 좌표
    FVector Max;                // 최대 좌표
    FVector Center;             // 중심점
    FVector Extent;             // 범위 (반경)
};
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay();
virtual void EndPlay();
virtual void Tick(float DeltaTime);

// 레벨 관리
ULevel* GetCurrentLevel() const;
void SetCurrentLevel(ULevel* Level);
ULevel* CreateLevel(const FName& LevelName);
void CreateDefaultLevel();

// 월드 상태
bool IsInitialized() const;
bool IsPlaying() const;
bool IsPaused() const;
void SetPlaying(bool bPlaying);
void SetPaused(bool bPaused);

// 초기화
virtual void InitializeWorld();
virtual void CleanupWorld();

// 액터 검색 (CurrentLevel로 위임)
AActor* FindActor(const FName& ActorName) const;
template<typename T>
TArray<T*> GetActorsOfClass() const;
TArray<AActor*> GetAllActors() const;
int32 GetTotalActorCount() const;

// 시간 관리
float GetTimeSeconds() const;
float GetRealTimeSeconds() const;
float GetDeltaTimeSeconds() const;
void SetTimeSeconds(float Time);
void AddDeltaTime(float DeltaTime);

// 월드 경계
const FWorldBounds& GetWorldBounds() const;
void SetWorldBounds(const FWorldBounds& Bounds);

// 싱글톤 접근
static UWorld* GetCurrentWorld();
static void SetCurrentWorld(UWorld* World);

// 팩토리
static UWorld* CreateWorld(const FName& WorldName);
static void DestroyWorld(UWorld* World);

// 저장/로드 (추후 구현)
virtual bool SaveWorld(const FString& FilePath);
virtual bool LoadWorld(const FString& FilePath);
```

#### 템플릿 구현 (World.h:125)
```cpp
template<typename T>
TArray<T*> UWorld::GetActorsOfClass() const
{
    if (CurrentLevel)
        return CurrentLevel->GetActorsOfClass<T>();
    return TArray<T*>();
}
```

#### 전역 접근 함수 (World.h:136)
```cpp
UWorld* GetWorld();                    // 전역 월드 얻기
void SetGlobalWorld(UWorld* World);    // 전역 월드 설정
```

#### 디자인 패턴
- **Singleton Pattern**: CurrentWorldInstance (전역 월드 접근)
- **Composite Pattern**: UWorld → ULevel → AActor 계층
- **Facade Pattern**: 레벨의 액터 검색 기능을 래핑

---

### 1.2 ULevel (Level.h:10)
**액터 컨테이너 - Container + Manager**

#### 역할
- 액터들의 컨테이너
- 액터 생명주기 관리
- 액터 검색 및 스폰
- 레벨 로드/언로드

#### 상속 관계
```
UObject
└── ULevel
```

#### 주요 멤버 변수
```cpp
// 액터 컨테이너
TArray<AActor*> Actors;         // 레벨 내 모든 액터

// 레벨 상태
bool bIsLoaded;                 // 로드 여부
bool bIsVisible;                // 가시성
bool bIsCurrent;                // 현재 레벨 여부

// 레벨 정보
FName LevelName;                // 레벨 이름
UWorld* OwningWorld;            // 소유 월드
```

#### 핵심 메서드
```cpp
// 생명주기
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
const TArray<AActor*>& GetActors() const;
int32 GetNumActors() const;

// 레벨 상태
bool IsLoaded() const;
bool IsVisible() const;
bool IsCurrent() const;
void SetLoaded(bool bLoaded);
void SetVisible(bool bVisible);
void SetCurrent(bool bCurrent);

// 월드 참조
UWorld* GetWorld() const;
void SetWorld(UWorld* World);

// 레벨 정보
const FName& GetLevelName() const;
void SetLevelName(const FName& Name);

// 레벨 로드/언로드
virtual void LoadLevel();
virtual void UnloadLevel();

// 초기화
virtual void InitializeLevel();
virtual void CleanupLevel();

// 액터 스폰
template<typename T>
T* SpawnActor(const FVector& Location, const FVector& Rotation, const FName& ActorName);

// 액터 제거
bool DestroyActor(AActor* Actor);

// 레벨 통계
FLevelStats GetLevelStats() const;

protected:
    // 이벤트
    virtual void NotifyActorAdded(AActor* Actor);
    virtual void NotifyActorRemoved(AActor* Actor);

    // 액터 등록
    void RegisterActor(AActor* Actor);
    void UnregisterActor(AActor* Actor);

private:
    // 헬퍼
    void ValidateActors();
    void CleanupNullActors();
```

#### FLevelStats (Level.h:77)
**레벨 통계 정보**

```cpp
struct FLevelStats
{
    int32 TotalActors;          // 전체 액터 수
    int32 StaticMeshActors;     // 정적 메시 액터 수
    int32 LightActors;          // 라이트 액터 수
    int32 OtherActors;          // 기타 액터 수
    size_t MemoryUsage;         // 메모리 사용량
};
```

#### 템플릿 구현 (Level.h:118-167)

##### FindActor<T> (Level.h:118)
```cpp
template<typename T>
T* ULevel::FindActor(const FName& ActorName) const
{
    for (AActor* Actor : Actors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            T* CastedActor = Cast<T>(Actor);
            if (CastedActor)
                return CastedActor;
        }
    }
    return nullptr;
}
```

##### GetActorsOfClass<T> (Level.h:135)
```cpp
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
                Result.push_back(CastedActor);
        }
    }
    return Result;
}
```

##### SpawnActor<T> (Level.h:155)
```cpp
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
```

#### 디자인 패턴
- **Container Pattern**: 액터들의 컨테이너
- **Factory Pattern**: SpawnActor<T>로 액터 생성
- **Observer Pattern**: NotifyActorAdded/Removed 이벤트

---

## 2. Actor System

### 2.1 AActor (Actor.h:8)
**게임 오브젝트 베이스 클래스 - Component Pattern**

#### 역할
- 게임 오브젝트 표현
- 컴포넌트 관리 (Composition)
- 트랜스폼 관리 (RootComponent 위임)
- 액터 계층 구조 (Owner/Owned)

#### 상속 관계
```
UObject
└── AActor
    └── AStaticMeshActor
```

#### 주요 멤버 변수
```cpp
// 액터 상태
bool bHidden;                   // 숨김 여부
bool bCanEverTick;              // Tick 가능 여부
bool bActorEnableCollision;     // 충돌 활성화
bool bBlockInput;               // 입력 차단

// 컴포넌트
TArray<UActorComponent*> Components;  // 모든 컴포넌트
USceneComponent* RootComponent;       // 루트 컴포넌트 (트랜스폼)

// 월드/레벨 참조
UWorld* World;                  // 소속 월드
ULevel* Level;                  // 소속 레벨

// 액터 계층 구조
AActor* Owner;                  // 소유자
TArray<AActor*> OwnedActors;    // 소유한 액터들
```

#### 핵심 메서드
```cpp
// 생명주기 (UObject 오버라이드)
virtual void BeginPlay() override;
virtual void EndPlay() override;
virtual void Tick(float DeltaTime) override;

// 트랜스폼 (RootComponent로 위임)
FVector GetActorLocation() const;
FVector GetActorRotation() const;
FVector GetActorScale() const;
void SetActorLocation(const FVector& NewLocation);
void SetActorRotation(const FVector& NewRotation);
void SetActorScale(const FVector& NewScale);
FMatrix GetActorTransform() const;
void SetActorTransform(const FMatrix& NewTransform);

// 액터 상태
bool IsHidden() const;
void SetHidden(bool bNewHidden);
bool CanEverTick() const;
void SetCanEverTick(bool bNewCanTick);

// 컴포넌트 관리
template<typename T>
T* CreateComponent(const FName& ComponentName);
template<typename T>
T* FindComponentByClass() const;
UActorComponent* FindComponentByName(const FName& ComponentName) const;
void AddComponent(UActorComponent* Component);
void RemoveComponent(UActorComponent* Component);
const TArray<UActorComponent*>& GetComponents() const;

// 루트 컴포넌트
USceneComponent* GetRootComponent() const;
void SetRootComponent(USceneComponent* NewRootComponent);

// 월드/레벨 상호작용
UWorld* GetWorld() const;
void SetWorld(UWorld* NewWorld);
ULevel* GetLevel() const;
void SetLevel(ULevel* NewLevel);

// 액터 계층 구조
AActor* GetOwner() const;
void SetOwner(AActor* NewOwner);
void GetOwnedActors(TArray<AActor*>& OutActors) const;
void AttachToActor(AActor* ParentActor);
void DetachFromActor();

protected:
    // 액터 이벤트
    virtual void NotifyActorOnClicked();
    virtual void NotifyHit(AActor* OtherActor, FVector HitLocation);
```

#### 템플릿 구현 (Actor.h:103-133)

##### CreateComponent<T> (Actor.h:103)
```cpp
template<typename T>
T* AActor::CreateComponent(const FName& ComponentName)
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must derive from UActorComponent");

    FName ActualComponentName = ComponentName.IsNone() ?
        FName(FString("DefaultComponent")) : ComponentName;

    T* NewComponent = NewObject<T>(this, ActualComponentName);
    if (NewComponent)
    {
        AddComponent(NewComponent);
    }
    return NewComponent;
}
```

##### FindComponentByClass<T> (Actor.h:119)
```cpp
template<typename T>
T* AActor::FindComponentByClass() const
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must derive from UActorComponent");

    for (UActorComponent* Component : Components)
    {
        T* TypedComponent = Cast<T>(Component);
        if (TypedComponent)
            return TypedComponent;
    }
    return nullptr;
}
```

#### 디자인 패턴
- **Component Pattern**: 기능을 컴포넌트로 분리
- **Composite Pattern**: 액터 계층 구조 (Owner/Owned)
- **Delegation Pattern**: 트랜스폼을 RootComponent에 위임

---

### 2.2 AStaticMeshActor (StaticMeshActor.h:11)
**정적 메시 렌더링 액터 - Convenience Class**

#### 역할
- 정적 메시 렌더링
- StaticMeshComponent 래핑
- 편의 함수 제공
- 프로시저럴 메시 액터 생성

#### 상속 관계
```
UObject
└── AActor
    └── AStaticMeshActor
```

#### 주요 멤버 변수
```cpp
UStaticMeshComponent* StaticMeshComponent;  // 메인 컴포넌트
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay() override;
virtual void EndPlay() override;
virtual void Tick(float DeltaTime) override;

// 컴포넌트 접근
UStaticMeshComponent* GetStaticMeshComponent() const;

// 정적 메시 관리 (StaticMeshComponent로 위임)
void SetStaticMesh(UStaticMesh* InStaticMesh);
UStaticMesh* GetStaticMesh() const;

// 머티리얼 관리 (StaticMeshComponent로 위임)
void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material);
UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
int32 GetNumMaterials() const;

// 렌더링 설정 (StaticMeshComponent로 위임)
void SetVisibility(bool bNewVisibility);
bool IsVisible() const;
void SetHidden(bool bNewHidden);
bool IsHidden() const;

// 바운딩 정보 (StaticMeshComponent로 위임)
FVector GetBoundingBoxMin() const;
FVector GetBoundingBoxMax() const;
FVector GetBoundingBoxCenter() const;
FVector GetBoundingBoxExtent() const;
float GetBoundingSphereRadius() const;
FBoxSphereBounds GetBounds() const;

// 메시 정보 (StaticMeshComponent로 위임)
uint32 GetNumVertices() const;
uint32 GetNumTriangles() const;
int32 GetNumSections() const;

// 정적 팩토리 메서드
static AStaticMeshActor* CreateWithMesh(UStaticMesh* InStaticMesh, const FVector& Location);
static AStaticMeshActor* CreateWithCubeMesh(const FVector& Location, const FVector& BoxRadius);
static AStaticMeshActor* CreateWithSphereMesh(const FVector& Location, float SphereRadius, int32 SphereSegments, int32 SphereRings);
static AStaticMeshActor* CreateWithPlaneMesh(const FVector& Location, const FVector& PlaneSize, int32 WidthSegments, int32 HeightSegments);
static AStaticMeshActor* CreateWithCylinderMesh(const FVector& Location, float CylinderRadius, float CylinderHeight, int32 CylinderSegments);
static AStaticMeshActor* CreateWithConeMesh(const FVector& Location, float ConeRadius, float ConeHeight, int32 ConeSegments);

protected:
    // 초기화
    virtual void InitializeActor();
    virtual void SetupComponents();

    // 유효성 검사
    bool IsStaticMeshComponentValid() const;

private:
    // 헬퍼
    void CreateStaticMeshComponent();
```

#### 디자인 패턴
- **Facade Pattern**: StaticMeshComponent 기능을 래핑
- **Factory Pattern**: CreateWithXXXMesh 정적 팩토리
- **Delegation Pattern**: 모든 기능을 StaticMeshComponent에 위임

---

## 3. Component Architecture

### 3.1 UActorComponent (ActorComponent.h:9)
**컴포넌트 베이스 클래스 - Component Pattern**

#### 역할
- 액터 기능의 재사용 가능한 단위
- 컴포넌트 생명주기 관리
- 활성화/비활성화 상태 관리

#### 상속 관계
```
UObject
└── UActorComponent
    └── USceneComponent
        └── UPrimitiveComponent
            └── UMeshComponent
                └── UStaticMeshComponent
```

#### 주요 멤버 변수
```cpp
// 컴포넌트 상태
bool bIsActive;                 // 활성화 여부
bool bCanEverTick;              // Tick 가능 여부
bool bWantsInitializeComponent; // 초기화 요청 여부
bool bHasBeenInitialized;       // 초기화 완료 여부
bool bRegistered;               // 등록 여부

// 자동 활성화
bool bAutoActivate;             // 자동 활성화

// 컴포넌트 태그
TArray<FString> ComponentTags;  // 검색용 태그
```

#### 핵심 메서드
```cpp
// 생명주기 (UObject 오버라이드)
virtual void BeginPlay() override;
virtual void EndPlay() override;
virtual void Tick(float DeltaTime) override;

// 컴포넌트 상태
bool IsActive() const;
void SetActive(bool bNewActive);
bool CanEverTick() const;
void SetCanEverTick(bool bNewCanTick);
bool IsRegistered() const;

// 소유자 액터
AActor* GetOwner() const;
template<typename T>
T* GetOwner() const;

// 컴포넌트 이벤트
virtual void OnRegister();
virtual void OnUnregister();
virtual void OnActivated();
virtual void OnDeactivated();

// 컴포넌트 초기화
virtual void InitializeComponent();
virtual void UninitializeComponent();

// 월드 상호작용
UWorld* GetWorld() const;

protected:
    // 내부 함수
    void RegisterComponent();
    void UnregisterComponent();
```

#### 템플릿 구현 (ActorComponent.h:73)
```cpp
template<typename T>
T* UActorComponent::GetOwner() const
{
    return Cast<T>(GetOuter());  // Outer는 소유자 액터
}
```

#### 디자인 패턴
- **Component Pattern**: 기능을 재사용 가능한 컴포넌트로 분리
- **Template Method**: 가상 함수로 확장 가능한 구조

---

### 3.2 USceneComponent (SceneComponent.h:7)
**트랜스폼 컴포넌트 - Composite Pattern**

#### 역할
- 3D 공간 트랜스폼 관리
- 컴포넌트 계층 구조 (부모/자식)
- 상대/절대 트랜스폼 변환
- 가시성 관리

#### 상속 관계
```
UObject
└── UActorComponent
    └── USceneComponent
```

#### 주요 멤버 변수
```cpp
// World Transform (절대 좌표)
FVector WorldLocation;
FVector WorldRotation;
FVector WorldScale;

// Relative Transform (부모 기준)
FVector RelativeLocation;
FVector RelativeRotation;
FVector RelativeScale;

// 컴포넌트 계층 구조
USceneComponent* AttachParent;          // 부모 컴포넌트
TArray<USceneComponent*> AttachChildren; // 자식 컴포넌트들

// 상태
bool bVisible;                  // 가시성
bool bAbsoluteLocation;         // 절대 위치 사용
bool bAbsoluteRotation;         // 절대 회전 사용
bool bAbsoluteScale;            // 절대 스케일 사용
```

#### 핵심 메서드
```cpp
// World Transform
FVector GetComponentLocation() const;
FVector GetComponentRotation() const;
FVector GetComponentScale() const;
void SetWorldLocation(const FVector& NewLocation);
void SetWorldRotation(const FVector& NewRotation);
void SetWorldScale(const FVector& NewScale);

// Relative Transform
FVector GetRelativeLocation() const;
FVector GetRelativeRotation() const;
FVector GetRelativeScale() const;
void SetRelativeLocation(const FVector& NewLocation);
void SetRelativeRotation(const FVector& NewRotation);
void SetRelativeScale(const FVector& NewScale);

// 컴포넌트 계층 구조
USceneComponent* GetAttachParent() const;
void AttachToComponent(USceneComponent* Parent);
void DetachFromComponent();
const TArray<USceneComponent*>& GetAttachChildren() const;

// 가시성
bool IsVisible() const;
void SetVisibility(bool bNewVisible);

protected:
    // Transform 업데이트
    virtual void UpdateWorldTransform();
    virtual void UpdateChildTransforms();

private:
    void AddChild(USceneComponent* Child);
    void RemoveChild(USceneComponent* Child);
```

#### 트랜스폼 변환 흐름
```
RelativeTransform
    ↓ (부모 기준)
ParentTransform × RelativeTransform
    ↓
WorldTransform
    ↓
UpdateChildTransforms() (재귀)
```

#### 디자인 패턴
- **Composite Pattern**: 부모/자식 계층 구조
- **Observer Pattern**: UpdateChildTransforms (자식들에게 전파)

---

### 3.3 UPrimitiveComponent (PrimitiveComponent.h:6)
**렌더링/물리 컴포넌트 베이스**

#### 역할
- 렌더링 가능한 컴포넌트
- 물리 상호작용 (추후 구현)
- 바운딩 박스/스피어 제공
- 머티리얼 관리

#### 상속 관계
```
UObject
└── UActorComponent
    └── USceneComponent
        └── UPrimitiveComponent
```

#### 주요 멤버 변수
```cpp
// 렌더링 플래그
bool bVisible;          // 가시성
bool bHidden;           // 숨김 (강제)
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay() override;
virtual void EndPlay() override;
virtual void Tick(float DeltaTime) override;

// 렌더링
virtual bool ShouldRender() const;
virtual void SetVisibility(bool bNewVisibility);
bool IsVisible() const;
virtual void SetHidden(bool bNewHidden);
bool IsHidden() const;

// 머티리얼 관리 (하위 클래스에서 구현)
virtual void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material);
virtual UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
virtual int32 GetNumMaterials() const;

// 바운딩 박스/스피어 (하위 클래스에서 구현)
virtual FVector GetBoundingBoxMin() const;
virtual FVector GetBoundingBoxMax() const;
virtual FVector GetBoundingBoxCenter() const;
virtual FVector GetBoundingBoxExtent() const;
virtual float GetBoundingSphereRadius() const;
virtual FBoxSphereBounds GetBounds() const;

// 레이캐스팅 (추후 구현)
virtual bool LineTraceComponent(const FVector& Start, const FVector& End, FVector& OutHitLocation) const;

protected:
    // 렌더 상태
    virtual void MarkRenderStateDirty();
    virtual void UpdateRenderState();
```

#### 렌더링 조건
```cpp
ShouldRender() = bVisible && !bHidden
```

#### 디자인 패턴
- **Template Method**: 하위 클래스에서 구현할 가상 함수 제공

---

### 3.4 UMeshComponent (MeshComponent.h:5)
**메시 렌더링 컴포넌트 베이스**

#### 역할
- 메시 렌더링 기능
- 머티리얼 오버라이드
- 바운딩 캐싱
- 와이어프레임 모드

#### 상속 관계
```
UObject
└── UActorComponent
    └── USceneComponent
        └── UPrimitiveComponent
            └── UMeshComponent
```

#### 주요 멤버 변수
```cpp
// 머티리얼 오버라이드
TArray<UMaterialInterface*> MaterialOverrides;  // 런타임 머티리얼 변경

// 렌더링 플래그
bool bWireframeMode;            // 와이어프레임 모드

// 바운딩 캐시
mutable FVector CachedBoundingBoxMin;
mutable FVector CachedBoundingBoxMax;
mutable float CachedBoundingSphereRadius;
mutable bool bBoundingCacheValid;
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay() override;
virtual void EndPlay() override;
virtual void Tick(float DeltaTime) override;

// 머티리얼 관리 (UPrimitiveComponent 오버라이드)
virtual void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material) override;
virtual UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
virtual int32 GetNumMaterials() const override;

// 머티리얼 오버라이드
void SetMaterialOverride(int32 MaterialIndex, UMaterialInterface* Material);
UMaterialInterface* GetMaterialOverride(int32 MaterialIndex) const;
void ClearMaterialOverrides();

// 바운딩 (UPrimitiveComponent 오버라이드)
virtual FVector GetBoundingBoxMin() const override;
virtual FVector GetBoundingBoxMax() const override;
virtual float GetBoundingSphereRadius() const override;
virtual FBoxSphereBounds GetBounds() const override;

// 메시 데이터 (하위 클래스에서 구현)
virtual uint32 GetNumVertices() const;
virtual uint32 GetNumTriangles() const;
virtual bool HasValidMeshData() const;

// 렌더링
virtual bool ShouldRender() const override;

// 와이어프레임
void SetWireframeMode(bool bWireframe);
bool IsWireframeMode() const;

protected:
    // 렌더 상태
    virtual void MarkRenderStateDirty() override;
    virtual void UpdateRenderState() override;

    // 바운딩 캐시
    virtual void InvalidateBounds();
    virtual void UpdateBounds() const;

    // 머티리얼 유틸리티
    UMaterialInterface* GetEffectiveMaterial(int32 MaterialIndex) const;
```

#### 머티리얼 우선순위
```
1. MaterialOverrides[Index] (런타임 오버라이드)
2. 원본 머티리얼 (UStaticMesh의 머티리얼)
```

#### 디자인 패턴
- **Decorator Pattern**: MaterialOverrides로 원본 머티리얼 장식
- **Lazy Evaluation**: 바운딩 캐시 (bBoundingCacheValid)

---

### 3.5 UStaticMeshComponent (StaticMeshComponent.h:10)
**정적 메시 렌더링 컴포넌트**

#### 역할
- UStaticMesh 렌더링
- 머티리얼 관리
- 인스턴싱 지원 (향후 확장)
- 바운딩 정보 제공

#### 상속 관계
```
UObject
└── UActorComponent
    └── USceneComponent
        └── UPrimitiveComponent
            └── UMeshComponent
                └── UStaticMeshComponent
```

#### 주요 멤버 변수
```cpp
// 메시 애셋
UStaticMesh* StaticMesh;        // 정적 메시 참조

// 인스턴싱 데이터
TArray<FMatrix> InstanceTransforms;  // 인스턴스 트랜스폼 배열
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void BeginPlay() override;
virtual void EndPlay() override;

// 정적 메시 관리
void SetStaticMesh(UStaticMesh* InStaticMesh);
UStaticMesh* GetStaticMesh() const;

// 머티리얼 관리 (UMeshComponent 오버라이드)
virtual void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material) override;
virtual UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
virtual int32 GetNumMaterials() const override;

// 메시 데이터 (UMeshComponent 오버라이드)
virtual uint32 GetNumVertices() const override;
virtual uint32 GetNumTriangles() const override;
virtual bool HasValidMeshData() const override;

// 바운딩 (UPrimitiveComponent 오버라이드)
virtual FVector GetBoundingBoxMin() const override;
virtual FVector GetBoundingBoxMax() const override;
virtual float GetBoundingSphereRadius() const override;
virtual FBoxSphereBounds GetBounds() const override;

// 섹션 관리
int32 GetNumSections() const;
const FStaticMeshSection* GetSection(int32 SectionIndex) const;

// 렌더 데이터
const FStaticMeshRenderData* GetRenderData() const;

// 인스턴싱 (향후 확장)
void SetInstanceData(const TArray<FMatrix>& InstanceTransforms);
const TArray<FMatrix>& GetInstanceData() const;
int32 GetInstanceCount() const;
bool IsInstanced() const;

protected:
    // UMeshComponent 오버라이드
    virtual void UpdateBounds() const override;
    virtual void MarkRenderStateDirty() override;
    virtual void UpdateRenderState() override;

    // 정적 메시 변경 처리
    virtual void OnStaticMeshChanged();

    // 머티리얼 유틸리티
    virtual UMaterialInterface* GetEffectiveMaterial(int32 MaterialIndex) const;

private:
    // 검증
    bool ValidateStaticMesh() const;
```

#### 인스턴싱 지원
```cpp
// 인스턴스 데이터 설정
TArray<FMatrix> Transforms;
for (int i = 0; i < 100; ++i)
{
    FMatrix InstanceTransform = ...;
    Transforms.push_back(InstanceTransform);
}
StaticMeshComp->SetInstanceData(Transforms);

// GPU Instancing으로 100개 메시 한 번에 렌더링
```

#### 디자인 패턴
- **Proxy Pattern**: UStaticMesh 애셋의 렌더링 프록시
- **Strategy Pattern**: 인스턴싱 vs 일반 렌더링

---

## Key Flows

### World → Level → Actor → Component 계층 흐름

```
1. 월드 생성
   UWorld* World = UWorld::CreateWorld("MyWorld");
   World->InitializeWorld();
   World->CreateDefaultLevel();

2. 레벨 생성
   ULevel* Level = World->CreateLevel("MyLevel");
   World->SetCurrentLevel(Level);

3. 액터 스폰
   AStaticMeshActor* Actor = Level->SpawnActor<AStaticMeshActor>(
       FVector(0, 0, 0),   // Location
       FVector::Zero,      // Rotation
       FName("MyActor")
   );

4. 컴포넌트 생성
   UStaticMeshComponent* MeshComp = Actor->GetStaticMeshComponent();
   UStaticMesh* Mesh = UKismetProceduralMeshLibrary::CreateCubeMesh();
   MeshComp->SetStaticMesh(Mesh);

5. 게임 루프
   World->BeginPlay();          // 모든 레벨/액터/컴포넌트 BeginPlay 호출
   while (bIsRunning)
   {
       World->Tick(DeltaTime);  // 모든 레벨/액터/컴포넌트 Tick 호출
   }
   World->EndPlay();            // 정리
```

### 액터 스폰 상세 흐름

```
ULevel::SpawnActor<T>(Location, Rotation, Name)
├─ 1. NewObject<T>(this, Name)
│   ├─ FObjectInitializer 생성
│   ├─ UClass::CreateDefaultObject()
│   ├─ T 생성자 호출
│   └─ GUObjectArray에 등록
│
├─ 2. Actor->SetActorLocation(Location)
│   └─ RootComponent->SetWorldLocation(Location)
│       └─ UpdateWorldTransform()
│
├─ 3. Actor->SetActorRotation(Rotation)
│   └─ RootComponent->SetWorldRotation(Rotation)
│
├─ 4. AddActor(Actor)
│   ├─ Actors.push_back(Actor)
│   ├─ Actor->SetLevel(this)
│   ├─ Actor->SetWorld(OwningWorld)
│   ├─ RegisterActor(Actor)
│   └─ NotifyActorAdded(Actor)
│
└─ 5. return Actor
```

### 컴포넌트 생성 및 어태치 흐름

```
Actor->CreateComponent<UStaticMeshComponent>("MeshComp")
├─ 1. NewObject<UStaticMeshComponent>(this, "MeshComp")
│   ├─ UStaticMeshComponent 생성자
│   │   ├─ UMeshComponent 생성자
│   │   │   ├─ UPrimitiveComponent 생성자
│   │   │   │   ├─ USceneComponent 생성자
│   │   │   │   │   └─ UActorComponent 생성자
│   │   │   │   │       └─ UObject 생성자
│   │   │   │   └─ bVisible = true, bHidden = false
│   │   │   └─ MaterialOverrides 초기화
│   │   └─ StaticMesh = nullptr
│   └─ Outer = Actor (소유자 설정)
│
├─ 2. AddComponent(NewComponent)
│   ├─ Components.push_back(NewComponent)
│   ├─ RegisterComponent()
│   │   ├─ bRegistered = true
│   │   └─ OnRegister() 호출
│   │
│   └─ if (USceneComponent* SceneComp = Cast<USceneComponent>(NewComponent))
│       {
│           if (RootComponent == nullptr)
│               SetRootComponent(SceneComp);  // 첫 SceneComponent를 루트로
│       }
│
└─ 3. return NewComponent
```

### 트랜스폼 업데이트 흐름 (계층 구조)

```
Actor->SetActorLocation(NewLocation)
├─ RootComponent->SetWorldLocation(NewLocation)
    ├─ WorldLocation = NewLocation
    ├─ UpdateWorldTransform()
    │   └─ RelativeLocation = (부모가 있으면) ParentTransform.Inverse() × WorldLocation
    │                         (부모가 없으면) WorldLocation
    │
    └─ UpdateChildTransforms() (재귀)
        └─ for (Child : AttachChildren)
            {
                Child->WorldLocation = this->WorldTransform × Child->RelativeLocation
                Child->UpdateWorldTransform()
                Child->UpdateChildTransforms()  // 재귀
            }
```

### BeginPlay/Tick/EndPlay 전파 흐름

```
UWorld::BeginPlay()
└─ CurrentLevel->BeginPlay()
    └─ for (Actor : Actors)
        {
            Actor->BeginPlay()
            └─ for (Component : Components)
                {
                    Component->BeginPlay()
                    └─ OnActivated() 호출 (if bAutoActivate)
                }
        }

UWorld::Tick(DeltaTime)
├─ AddDeltaTime(DeltaTime)  // TimeSeconds 갱신
└─ CurrentLevel->Tick(DeltaTime)
    └─ for (Actor : Actors)
        {
            if (Actor->CanEverTick())
                Actor->Tick(DeltaTime)
                └─ for (Component : Components)
                    {
                        if (Component->CanEverTick() && Component->IsActive())
                            Component->Tick(DeltaTime)
                    }
        }

UWorld::EndPlay()
└─ CurrentLevel->EndPlay()
    └─ for (Actor : Actors)
        {
            Actor->EndPlay()
            └─ for (Component : Components)
                {
                    Component->EndPlay()
                    └─ OnDeactivated() 호출
                }
        }
```

### 액터 검색 흐름 (타입 기반)

```
World->GetActorsOfClass<AStaticMeshActor>()
├─ CurrentLevel->GetActorsOfClass<AStaticMeshActor>()
    └─ TArray<AStaticMeshActor*> Result
       for (Actor : Actors)
       {
           if (Actor)
           {
               AStaticMeshActor* CastedActor = Cast<AStaticMeshActor>(Actor)
               ├─ Actor->IsA(AStaticMeshActor::GetStaticClass())
               │   └─ Actor->GetClass()->IsChildOf(AStaticMeshActor::GetStaticClass())
               │       └─ RTTI 체크 (재귀적으로 부모 클래스 확인)
               │
               └─ if (CastedActor)
                   Result.push_back(CastedActor)
           }
       }
       return Result
```

---

## Design Patterns Summary

### 1. Singleton Pattern
- **UWorld::CurrentWorldInstance**: 전역 월드 접근
- **GetWorld() / SetGlobalWorld()**: 전역 접근 함수

### 2. Composite Pattern
- **UWorld → ULevel → AActor**: 게임 세계 계층
- **AActor → UActorComponent**: 액터 컴포넌트 구성
- **USceneComponent**: 부모/자식 계층 구조
- **AActor Owner/Owned**: 액터 소유 관계

### 3. Component Pattern
- **UActorComponent**: 기능을 재사용 가능한 컴포넌트로 분리
- **AActor**: 컴포넌트 컨테이너

### 4. Factory Pattern
- **UWorld::CreateWorld()**: 월드 생성
- **UWorld::CreateLevel()**: 레벨 생성
- **ULevel::SpawnActor<T>()**: 액터 스폰
- **AActor::CreateComponent<T>()**: 컴포넌트 생성
- **AStaticMeshActor::CreateWithXXXMesh()**: 정적 팩토리

### 5. Template Method Pattern
- **UActorComponent**: 가상 함수로 확장 가능한 구조
- **BeginPlay/Tick/EndPlay**: 생명주기 훅

### 6. Observer Pattern
- **ULevel::NotifyActorAdded/Removed**: 액터 추가/제거 이벤트
- **UActorComponent::OnRegister/OnUnregister**: 컴포넌트 이벤트
- **USceneComponent::UpdateChildTransforms()**: 트랜스폼 전파

### 7. Delegation Pattern
- **AActor 트랜스폼**: RootComponent에 위임
- **AStaticMeshActor**: StaticMeshComponent에 모든 기능 위임

### 8. Facade Pattern
- **UWorld**: 레벨 기능 래핑 (GetActorsOfClass 등)
- **AStaticMeshActor**: StaticMeshComponent 래핑

### 9. Proxy Pattern
- **UStaticMeshComponent**: UStaticMesh 애셋의 렌더링 프록시

### 10. Decorator Pattern
- **UMeshComponent::MaterialOverrides**: 원본 머티리얼 장식

### 11. Lazy Evaluation Pattern
- **UMeshComponent**: 바운딩 캐시 (bBoundingCacheValid)

---

## Class Hierarchy Diagram

```
┌────────────────────────────────────────────────────────────┐
│                      Engine Core                            │
└────────────────────────────────────────────────────────────┘
                          │
        ┌─────────────────┼─────────────────────┐
        │                 │                     │
        ▼                 ▼                     ▼
  ┌──────────┐      ┌──────────┐      ┌──────────────┐
  │  World   │      │  Actor   │      │  Component   │
  └──────────┘      └──────────┘      └──────────────┘
        │                 │                     │
        ▼                 ▼                     ▼
    UWorld            AActor            UActorComponent
        │                 │                     │
        ▼                 ▼                     ▼
    ULevel      AStaticMeshActor      USceneComponent
                                               │
                                               ▼
                                      UPrimitiveComponent
                                               │
                                               ▼
                                        UMeshComponent
                                               │
                                               ▼
                                    UStaticMeshComponent
```

### 상세 클래스 계층

```
UObject
├── UWorld
│   └── ULevel (Composite)
│       └── TArray<AActor*>
│
├── AActor (Component Container)
│   ├── TArray<UActorComponent*>
│   ├── USceneComponent* RootComponent
│   ├── UWorld* World
│   ├── ULevel* Level
│   ├── AActor* Owner
│   └── TArray<AActor*> OwnedActors
│
├── AStaticMeshActor : AActor
│   └── UStaticMeshComponent* StaticMeshComponent
│
├── UActorComponent
│   └── USceneComponent (Transform)
│       ├── USceneComponent* AttachParent
│       ├── TArray<USceneComponent*> AttachChildren
│       └── UPrimitiveComponent (Rendering + Physics)
│           └── UMeshComponent (Mesh Rendering)
│               ├── TArray<UMaterialInterface*> MaterialOverrides
│               └── UStaticMeshComponent
│                   ├── UStaticMesh* StaticMesh
│                   └── TArray<FMatrix> InstanceTransforms
│
└── UStaticMesh (Asset)
    ├── FStaticMeshRenderData
    ├── TArray<FStaticMaterial>
    └── TArray<FStaticMeshSection>
```

---

## 다른 시스템과의 관계

### Engine Core → Core Systems
- UWorld, ULevel, AActor, UActorComponent 모두 UObject 상속
- RTTI 시스템 사용 (Cast, IsA)
- FName 사용 (액터/컴포넌트 이름)
- GUObjectArray에서 액터 검색

### Engine Core → Rendering Pipeline
- UStaticMeshComponent가 UStaticMesh 참조
- UPrimitiveComponent가 UMaterialInterface 관리
- 렌더러가 UWorld에서 렌더 가능한 컴포넌트 수집
- FSceneView와 함께 렌더링 수행

### Engine Core → Editor
- 에디터가 UWorld/ULevel 조작
- 뷰포트가 액터 선택/트랜스폼 편집
- 컴포넌트 편집 인터페이스 제공

---

## 참고: 클래스 매핑

| 분류 | 클래스/구조체 | 주요 역할 |
|-----|-------------|---------|
| **World** | UWorld | 최상위 게임 월드 |
| **World** | ULevel | 액터 컨테이너 |
| **World** | FWorldBounds | 월드 경계 |
| **World** | FLevelStats | 레벨 통계 |
| **Actor** | AActor | 게임 오브젝트 베이스 |
| **Actor** | AStaticMeshActor | 정적 메시 액터 |
| **Component** | UActorComponent | 컴포넌트 베이스 |
| **Component** | USceneComponent | 트랜스폼 컴포넌트 |
| **Component** | UPrimitiveComponent | 렌더링/물리 컴포넌트 |
| **Component** | UMeshComponent | 메시 컴포넌트 |
| **Component** | UStaticMeshComponent | 정적 메시 컴포넌트 |

---

## 학습 체크리스트

- [ ] World → Level → Actor → Component 계층 구조 이해
- [ ] 싱글톤 패턴으로 전역 월드 접근 이해
- [ ] 액터 스폰 흐름 (SpawnActor<T>) 이해
- [ ] 컴포넌트 패턴 이해 (Composition over Inheritance)
- [ ] 트랜스폼 계층 구조 이해 (부모/자식 변환)
- [ ] BeginPlay/Tick/EndPlay 전파 흐름 이해
- [ ] 액터/컴포넌트 검색 메커니즘 이해
- [ ] 머티리얼 오버라이드 시스템 이해
- [ ] RootComponent 위임 패턴 이해
- [ ] 인스턴싱 지원 구조 이해

---

**다음 단계**: Phase 4 - Editor 분석
