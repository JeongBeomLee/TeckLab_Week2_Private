# BeomsEngine - 전체 시스템 개요

## 프로젝트 정보

### 좌표계
- **Graphics API**: DirectX11
- **Coordinate System**: Z-Up Left-handed

### 아키텍처 원칙
- Unreal Engine 구조 모방
- Object-Oriented Programming (OOP)
- Component-Based Architecture
- 필요에 따른 적절한 타협 (Over-engineering 방지)

---

## 전체 시스템 아키텍처

```
BeomsEngine
├── Phase 1: Core Systems (기반 시스템)
│   ├── Object System (UObject, UClass, RTTI)
│   ├── Memory Management (GC, UObjectArray)
│   └── Reflection (매크로 기반 메타프로그래밍)
│
├── Phase 2: Rendering Pipeline (렌더링)
│   ├── RHI Layer (DirectX11 추상화)
│   ├── Renderer (렌더 패스 시스템)
│   ├── Material System (PBR)
│   └── Mesh System (정적 메시)
│
├── Phase 3: Engine Core (게임 세계)
│   ├── World Management (UWorld, ULevel)
│   ├── Actor System (AActor, Component Pattern)
│   └── Component Architecture (5단계 계층)
│
└── Phase 4: Editor (에디터)
    ├── Slate UI Framework (선언적 UI)
    ├── Viewport System (에디터 뷰포트)
    ├── Level Editor (레벨 편집)
    └── Event System (델리게이트)
```

---

## 시스템 간 의존성 그래프

```
┌─────────────────────────────────────────────────────────────┐
│                    Dependency Graph                          │
└─────────────────────────────────────────────────────────────┘

                    ┌──────────────┐
                    │ Phase 4      │
                    │ Editor       │
                    └──────┬───────┘
                           │ depends on
            ┌──────────────┼──────────────┐
            │              │              │
            ▼              ▼              ▼
    ┌──────────┐   ┌──────────┐   ┌──────────┐
    │ Phase 3  │   │ Phase 2  │   │ Phase 1  │
    │ Engine   │◄──│ Rendering│◄──│ Core     │
    │ Core     │   │ Pipeline │   │ Systems  │
    └──────────┘   └──────────┘   └──────────┘
         │              │              │
         └──────────────┴──────────────┘
                    모두 Phase 1에 의존
```

### 의존성 설명

1. **Phase 1 (Core Systems)**: 모든 시스템의 기반
   - UObject, UClass, FName 등 기본 타입
   - RTTI 시스템
   - GC 및 메모리 관리

2. **Phase 2 (Rendering Pipeline)**: Phase 1 의존
   - URenderer는 UObject 상속
   - UStaticMesh는 UObject 상속
   - FName을 머티리얼 이름으로 사용

3. **Phase 3 (Engine Core)**: Phase 1, 2 의존
   - UWorld, ULevel, AActor 모두 UObject 상속
   - UStaticMeshComponent가 UStaticMesh 참조
   - 렌더링 시스템과 통합

4. **Phase 4 (Editor)**: 모든 Phase 의존
   - FViewportClient가 FSceneView 생성
   - SLevelViewport가 UWorld/ULevel 참조
   - 델리게이트가 UObject 메서드 바인딩

---

## 핵심 클래스 계층도 (전체)

### 1. UObject 계층 (Core Systems 기반)

```
UObject (Root)
├── UWorld
│   └── ULevel
├── AActor
│   └── AStaticMeshActor
├── UActorComponent
│   └── USceneComponent
│       └── UPrimitiveComponent
│           └── UMeshComponent
│               └── UStaticMeshComponent
├── URenderer
└── UStaticMesh
```

### 2. Slate UI 계층 (독립적)

```
SWidget (Abstract)
├── SLeafWidget
│   ├── SButton
│   ├── STextBlock
│   └── SEditorViewport
└── SPanel
    ├── SHorizontalBox
    │   └── SLevelViewportToolbar
    └── SVerticalBox
        └── SLevelViewport
```

### 3. 기타 주요 클래스

```
FD3D11GraphicsDevice (RHI)
FViewportClient (Viewport)
└── FEditorViewportClient

TDelegate / TMulticastDelegate (Event System)
```

---

## 주요 디자인 패턴 정리

### 전체 시스템에서 사용된 패턴

| 패턴 | 사용 위치 | 목적 |
|------|----------|------|
| **Singleton** | UWorld, FNamePool, GUObjectArray | 전역 접근 |
| **Factory** | NewObject, SpawnActor, CreateWithXXX | 객체 생성 |
| **Component** | AActor + UActorComponent | Composition over Inheritance |
| **Composite** | UWorld→ULevel→AActor, SWidget 계층 | 계층 구조 |
| **Observer** | TMulticastDelegate, UpdateChildTransforms | 이벤트 전파 |
| **Strategy** | IRenderPass, ECameraMode | 알고리즘 교체 |
| **Template Method** | UObject 생명주기, SWidget 레이아웃 | 확장 가능한 구조 |
| **Facade** | FD3D11GraphicsDevice, AStaticMeshActor | 복잡한 인터페이스 단순화 |
| **Lazy Evaluation** | FSceneView 행렬, SWidget DesiredSize | 필요할 때만 계산 |
| **Flyweight** | FName (String Interning) | 메모리 절약 |
| **Delegation** | AActor 트랜스폼, AStaticMeshActor | 기능 위임 |
| **Command** | TDelegate, IRenderPass | 명령 객체화 |
| **Prototype** | UMaterialInstanceInterface | 복제 및 수정 |
| **Decorator** | UMeshComponent MaterialOverrides | 기능 추가 |
| **Mediator** | FEditorViewportClient | 중재자 |
| **Adapter** | SEditorViewport 이벤트 변환 | 인터페이스 변환 |
| **Fluent Interface** | FSlotBase, FObjectInitializer | Method Chaining |
| **RAII** | ComPtr | 자동 리소스 관리 |

---

## 핵심 실행 흐름

### 1. 엔진 초기화 전체 흐름

```
main()
├─ 1. DirectX11 초기화
│   └─ FD3D11GraphicsDevice::Init()
│       ├─ CreateDeviceAndContext()
│       ├─ CreateSwapChain()
│       ├─ CreateRenderTargetView()
│       ├─ CreateDepthStencilBuffer()
│       └─ CreateDepthStencilView()
│
├─ 2. 렌더러 초기화
│   └─ URenderer::InitializeRenderer()
│       └─ SetupDefaultRenderPasses()
│           ├─ AddRenderPass(new FDepthPrePass())
│           └─ AddRenderPass(new FBasePass())
│
├─ 3. 월드 생성
│   └─ UWorld::CreateWorld("MyWorld")
│       ├─ NewObject<UWorld>()
│       ├─ InitializeWorld()
│       └─ CreateDefaultLevel()
│           └─ ULevel::CreateLevel("DefaultLevel")
│
├─ 4. 레벨 구성
│   └─ Level->SpawnActor<AStaticMeshActor>()
│       ├─ NewObject<AStaticMeshActor>()
│       ├─ Actor->CreateComponent<UStaticMeshComponent>()
│       │   └─ StaticMeshComp->SetStaticMesh(Mesh)
│       └─ Level->AddActor(Actor)
│
├─ 5. 에디터 UI 초기화
│   └─ SLevelViewport::Initialize()
│       ├─ CreateViewportToolbar()
│       ├─ CreateEditorViewport()
│       │   └─ SetViewportClient(new FEditorViewportClient())
│       └─ SetLevel(CurrentLevel)
│
└─ 6. 게임 루프 시작
    └─ while (bIsRunning)
        ├─ ProcessInput()
        ├─ World->Tick(DeltaTime)
        ├─ SLevelViewport->Tick()
        │   └─ RenderViewport()
        └─ GraphicsDevice->SwapBuffer()
```

### 2. 프레임 렌더링 전체 흐름

```
Frame Loop
├─ 1. 입력 처리
│   └─ Windows Message Loop
│       └─ SLevelViewport->OnMouseMove/OnMouseButtonDown()
│           └─ FEditorViewportClient->ProcessMouseInput()
│               └─ UpdateOrbitCamera()
│
├─ 2. 논리 업데이트 (Tick)
│   └─ World->Tick(DeltaTime)
│       └─ Level->Tick(DeltaTime)
│           └─ for (Actor : Actors)
│               └─ Actor->Tick(DeltaTime)
│                   └─ for (Component : Components)
│                       └─ Component->Tick(DeltaTime)
│
├─ 3. 카메라 업데이트
│   └─ FEditorViewportClient::UpdateSceneView()
│       ├─ UpdateOrbitCamera()
│       │   └─ CameraPos = CalculateOrbitPosition()
│       └─ SceneView->UpdateMatrices()
│           ├─ UpdateViewMatrix()
│           └─ UpdateProjectionMatrix()
│
├─ 4. 렌더링
│   └─ URenderer::RenderSceneWithView(SceneView)
│       ├─ FRenderPassContext 생성
│       │
│       ├─ ExecuteRenderPass(DepthPrePass)
│       │   ├─ ClearDepthStencil()
│       │   ├─ SetRenderTargets(nullptr, DSV)
│       │   └─ 모든 메시 그리기 (깊이만)
│       │
│       └─ ExecuteRenderPass(BasePass)
│           ├─ SetRenderTargets(RTV, DSV)
│           ├─ GetActorsOfClass<AStaticMeshActor>()
│           └─ for (Actor : Actors)
│               └─ StaticMeshComp = Actor->GetStaticMeshComponent()
│                   └─ for (Section : Sections)
│                       ├─ SetShader(Material->ShadingModel)
│                       ├─ UpdateConstantBuffer(VP, World, Material)
│                       └─ DrawIndexed()
│
├─ 5. UI 렌더링
│   └─ SLevelViewport->OnPaint()
│       ├─ ViewportToolbar->OnPaint()
│       └─ EditorViewport->OnPaint()
│
└─ 6. 버퍼 스왑
    └─ GraphicsDevice->SwapBuffer()
```

### 3. 액터 스폰 및 컴포넌트 생성 흐름

```
Level->SpawnActor<AStaticMeshActor>(Location, Rotation, Name)
│
├─ 1. 액터 생성
│   └─ NewObject<AStaticMeshActor>(Level, Name)
│       ├─ FObjectInitializer 생성
│       ├─ UClass::CreateDefaultObject()
│       ├─ AStaticMeshActor 생성자 호출
│       │   ├─ AActor 생성자
│       │   │   └─ UObject 생성자
│       │   │       ├─ UniqueID 생성
│       │   │       ├─ GUObjectArray::AllocateUObjectIndex()
│       │   │       └─ Outer = Level
│       │   │
│       │   └─ CreateStaticMeshComponent()
│       │       └─ StaticMeshComponent = CreateComponent<UStaticMeshComponent>()
│       │           ├─ NewObject<UStaticMeshComponent>(this, Name)
│       │           │   └─ UStaticMeshComponent 생성자
│       │           │       └─ ... (5단계 생성자 체인)
│       │           │
│       │           └─ AddComponent(StaticMeshComponent)
│       │               └─ Components.push_back(StaticMeshComponent)
│       │                   └─ if (!RootComponent)
│       │                       SetRootComponent(StaticMeshComponent)
│       │
│       └─ return Actor
│
├─ 2. 트랜스폼 설정
│   ├─ Actor->SetActorLocation(Location)
│   │   └─ RootComponent->SetWorldLocation(Location)
│   │       └─ UpdateWorldTransform()
│   │
│   └─ Actor->SetActorRotation(Rotation)
│       └─ RootComponent->SetWorldRotation(Rotation)
│
├─ 3. 레벨에 등록
│   └─ Level->AddActor(Actor)
│       ├─ Actors.push_back(Actor)
│       ├─ Actor->SetLevel(this)
│       ├─ Actor->SetWorld(OwningWorld)
│       ├─ RegisterActor(Actor)
│       └─ NotifyActorAdded(Actor)
│
└─ 4. return Actor
```

### 4. UI 레이아웃 및 이벤트 흐름

```
SLevelViewport UI 구성
│
├─ 1. UI 구축 (선언적)
│   └─ SLevelViewport::Initialize()
│       ├─ CreateViewportToolbar()
│       │   └─ ViewportToolbar = MakeShared<SLevelViewportToolbar>()
│       │       └─ AddSlot()[ViewModeButton]
│       │           AddSlot()[CameraModeButton]
│       │           AddSlot()[StatsText]
│       │
│       └─ CreateEditorViewport()
│           └─ EditorViewport = MakeShared<SEditorViewport>()
│
├─ 2. 레이아웃 계산 (Prepass)
│   └─ SLevelViewport->SlatePrepass()
│       ├─ if (bNeedsDesiredSizeCompute)
│       │   └─ CacheDesiredSize()
│       │       └─ ComputeDesiredSize()
│       │           ├─ ToolbarSize = Toolbar->ComputeDesiredSize()
│       │           ├─ ViewportSize = Viewport->ComputeDesiredSize()
│       │           └─ return (ToolbarSize.Y + ViewportSize.Y, max(Width))
│       │
│       └─ for (Child : Children)
│           └─ Child->SlatePrepass()  // 재귀
│
├─ 3. 자식 배치 (ArrangeChildren)
│   └─ SLevelViewport->ArrangeChildren(AllottedGeometry)
│       ├─ ToolbarGeometry = { Position: (0, 0), Size: (Width, 30) }
│       ├─ ViewportGeometry = { Position: (0, 30), Size: (Width, Height-30) }
│       └─ ArrangedChildren = [Toolbar, Viewport]
│
├─ 4. 렌더링 (OnPaint)
│   └─ SLevelViewport->OnPaint(PaintArgs)
│       ├─ Toolbar->OnPaint()
│       │   ├─ ViewModeButton->OnPaint()
│       │   ├─ CameraModeButton->OnPaint()
│       │   └─ StatsText->OnPaint()
│       │
│       └─ EditorViewport->OnPaint()
│           └─ RenderViewport()
│               └─ ViewportClient->Draw()
│
└─ 5. 이벤트 처리
    └─ User Click on ViewModeButton
        └─ SLevelViewport->OnMouseButtonDown(Geometry, PointerEvent)
            └─ Toolbar->OnMouseButtonDown(Geometry, PointerEvent)
                └─ ViewModeButton->OnMouseButtonDown(Geometry, PointerEvent)
                    ├─ bIsPressed = true
                    └─ return true  // 이벤트 소비

        └─ OnMouseButtonUp()
            └─ ViewModeButton->OnMouseButtonUp()
                └─ ExecuteOnClick()
                    └─ OnClicked.Execute()
                        └─ // 바인딩된 람다 실행
                            SetViewMode(NextMode)
```

---

## 메모리 관리 및 GC

### GUObjectArray를 통한 객체 추적

```
모든 UObject 생성 시:
    UObject 생성자
    └─ GUObjectArray::AllocateUObjectIndex(this)
        ├─ if (ObjectAvailableList.empty())
        │   {
        │       ObjectList.push_back(FUObjectItem(this))
        │       Index = ObjectList.size() - 1
        │   }
        │ else
        │   {
        │       Index = ObjectAvailableList.pop_back()
        │       ObjectList[Index] = FUObjectItem(this)
        │   }
        └─ this->SetInternalIndex(Index)

Garbage Collection (Mark-and-Sweep):
    GUObjectArray::PerformGarbageCollector()
    ├─ Mark Phase
    │   ├─ 모든 객체의 마크 플래그 초기화
    │   ├─ 루트 객체 수집 (World, Level 등)
    │   └─ for (Root : RootObjects)
    │       └─ MarkObjectAndReferences(Root)  // 재귀 마킹
    │
    └─ Sweep Phase
        └─ for (Item : ObjectList)
            {
                if (!Item.bMarked && Item.IsValid())
                {
                    delete Item.Object
                    ObjectAvailableList.push_back(Index)
                    Item.Object = nullptr
                }
            }
```

---

## RTTI 시스템

### 클래스 등록 및 타입 체크

```
클래스 정의:
    // Actor.h
    class AActor : public UObject
    {
        UCLASS()
        GENERATED_BODY(AActor, UObject)
        ...
    };

    // Actor.cpp
    IMPLEMENT_CLASS(AActor, UObject)

    → 확장됨:
    UClass* AActor::GetStaticClass()
    {
        static UClass* StaticClass = nullptr;
        if (!StaticClass)
        {
            StaticClass = new UClass(
                FName("AActor"),
                UObject::GetStaticClass(),  // SuperClass
                &AActor::CreateInstance
            );
            UClass::RegisterClass(StaticClass);  // 전역 레지스트리 등록
        }
        return StaticClass;
    }

타입 체크:
    UObject* Obj = ...;
    if (Obj->IsA<AActor>())
    {
        AActor* Actor = Cast<AActor>(Obj);
        // ...
    }

    → 내부 동작:
    Obj->IsA<AActor>()
    └─ Obj->IsA(AActor::GetStaticClass())
        └─ Obj->GetClass()->IsChildOf(AActor::GetStaticClass())
            └─ 재귀적으로 SuperClass 체인 확인
                if (this == SomeBase) return true;
                if (SuperClass) return SuperClass->IsChildOf(SomeBase);
                return false;
```

---

## 주요 최적화 기법

### 1. String Interning (FName)
```
문자열 중복 제거:
    FName("MyActor")  // 첫 번째
    FName("MyActor")  // 두 번째 (같은 인덱스 재사용)

    → FNamePool에서 "myactor" → ID 매핑
    → 메모리 절약 + 비교 O(1) (정수 비교)
```

### 2. Lazy Evaluation
```
FSceneView 행렬 계산:
    GetViewMatrix()
    └─ if (bViewMatrixDirty)
        {
            UpdateViewMatrix()  // 필요할 때만 계산
            bViewMatrixDirty = false
        }
        return ViewMatrix
```

### 3. Dirty Flag
```
SWidget 레이아웃:
    SetText("NewText")
    └─ InvalidateLayout()
        └─ bNeedsDesiredSizeCompute = true

    다음 프레임:
        SlatePrepass()
        └─ if (bNeedsDesiredSizeCompute)
            CacheDesiredSize()  // 재계산
```

### 4. Object Pooling (부분적)
```
GUObjectArray::ObjectAvailableList
    → 삭제된 객체 인덱스 재사용
    → 배열 크기 증가 최소화
```

### 5. Z-PrePass (Early Z Rejection)
```
1. DepthPrePass: 깊이만 렌더링 (픽셀 셰이더 없음)
2. BasePass: 깊이 테스트로 오버드로우 감소
    → 픽셀 셰이더 호출 횟수 감소
```

### 6. Component 바운딩 캐시
```
UMeshComponent:
    mutable FVector CachedBoundingBoxMin;
    mutable bool bBoundingCacheValid;

    GetBoundingBoxMin()
    └─ if (!bBoundingCacheValid)
        {
            UpdateBounds()  // 캐시 갱신
            bBoundingCacheValid = true
        }
        return CachedBoundingBoxMin
```

---

## 확장 포인트

### 새로운 액터 타입 추가

```cpp
// 1. 헤더 정의
class AMyCustomActor : public AActor
{
    UCLASS()
    GENERATED_BODY(AMyCustomActor, AActor)

public:
    AMyCustomActor();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UMyCustomComponent* CustomComponent;
};

// 2. 구현 파일
IMPLEMENT_CLASS(AMyCustomActor, AActor)

AMyCustomActor::AMyCustomActor()
{
    CustomComponent = CreateComponent<UMyCustomComponent>("CustomComp");
}
```

### 새로운 컴포넌트 타입 추가

```cpp
class UMyCustomComponent : public USceneComponent
{
    UCLASS()
    GENERATED_BODY(UMyCustomComponent, USceneComponent)

public:
    virtual void Tick(float DeltaTime) override;
};

IMPLEMENT_CLASS(UMyCustomComponent, USceneComponent)
```

### 새로운 렌더 패스 추가

```cpp
class FMyCustomPass : public IRenderPass
{
public:
    virtual void Execute(const FRenderPassContext& Context) override
    {
        // 커스텀 렌더링 로직
    }

    virtual ERenderPassType GetPassType() const override { return ERenderPassType::Custom; }
    virtual const char* GetPassName() const override { return "MyCustomPass"; }
};

// 렌더러에 추가
Renderer->AddRenderPass(new FMyCustomPass());
```

### 새로운 UI 위젯 추가

```cpp
class SMyCustomWidget : public SLeafWidget
{
public:
    virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float DeltaTime) const override;
};

// 사용
TSharedPtr<SMyCustomWidget> Widget = MakeShared<SMyCustomWidget>();
ParentPanel->AddSlot()[Widget];
```

---

## 학습 로드맵

### 초급
1. ✓ 프로젝트 구조 파악
2. ✓ Core Systems 이해 (UObject, UClass, RTTI)
3. ✓ 기본 타입 이해 (FVector, FMatrix, FName)
4. ✓ Actor와 Component 관계 이해

### 중급
5. ✓ 렌더링 파이프라인 흐름 이해
6. ✓ 머티리얼 시스템 이해
7. ✓ World/Level/Actor 계층 이해
8. ✓ Slate UI 레이아웃 시스템 이해
9. ✓ 델리게이트 및 이벤트 시스템 이해

### 고급
10. GC 알고리즘 깊이 이해 (Mark-and-Sweep)
11. RTTI 매크로 시스템 확장
12. 커스텀 렌더 패스 구현
13. 복잡한 UI 레이아웃 구현
14. 에디터 도구 확장
15. 최적화 기법 적용

---

## 참고 문서

### Phase별 상세 문서
1. [Phase1_CoreSystems.md](Phase1_CoreSystems.md) - Object System, Memory, Reflection
2. [Phase2_RenderingPipeline.md](Phase2_RenderingPipeline.md) - RHI, Renderer, Material, Scene
3. [Phase3_EngineCore.md](Phase3_EngineCore.md) - World/Level/Actor, Component
4. [Phase4_Editor.md](Phase4_Editor.md) - Slate UI, Viewport, Editor Tools

### 외부 참고 자료
- Unreal Engine Documentation: https://docs.unrealengine.com/
- DirectX 11 Documentation: https://docs.microsoft.com/en-us/windows/win32/direct3d11/
- Game Engine Architecture (Jason Gregory)

---

## 프로젝트 통계

### 코드 구조
```
BeomsEngine/
├── Main (진입점)
├── Core (17 classes)
│   ├── Object System (7 classes)
│   ├── Containers (8 classes)
│   └── Memory (2 classes)
├── Rendering (10 classes)
│   ├── RHI (1 class)
│   ├── Renderer (4 classes)
│   ├── Material (2 classes)
│   └── Mesh (3 classes)
├── Engine Core (11 classes)
│   ├── World/Level (2 classes)
│   ├── Actor (2 classes)
│   └── Components (7 classes)
└── Editor (12 classes)
    ├── Slate UI (8 classes)
    ├── Viewport (3 classes)
    └── Event (1 class)

Total: ~50 classes
```

### 디자인 패턴 사용 빈도
- Singleton: 3회
- Factory: 10회
- Component: 7회 (핵심)
- Composite: 5회
- Observer: 5회
- Strategy: 4회
- Template Method: 6회
- Lazy Evaluation: 3회
- Facade: 3회
- 기타: 10여 개

---

## 결론

BeomsEngine은 Unreal Engine의 핵심 아키텍처를 학습 목적으로 재구현한 프로젝트입니다.

### 핵심 성과
1. ✓ Component-Based Architecture 구현
2. ✓ RTTI 시스템 구현 (매크로 기반)
3. ✓ Garbage Collection (Mark-and-Sweep)
4. ✓ DirectX11 기반 렌더링 파이프라인
5. ✓ Slate UI 프레임워크 (선언적 UI)
6. ✓ 델리게이트 이벤트 시스템
7. ✓ 에디터 뷰포트 및 카메라 시스템

### 주요 학습 포인트
- Object-Oriented Design
- Component Pattern의 실전 적용
- 렌더링 파이프라인 설계
- 이벤트 기반 아키텍처
- UI 레이아웃 시스템
- 메모리 관리 및 GC

### 향후 확장 가능 영역
- Physics System (충돌 감지, 물리 시뮬레이션)
- Animation System (스켈레탈 메시, 애니메이션 블렌딩)
- Audio System (사운드 재생, 3D 오디오)
- Networking (멀티플레이어)
- Scripting (Lua/Python 통합)
- Asset Pipeline (임포터, 에디터 통합)
- Advanced Rendering (PBR, 라이팅, 그림자, 포스트 프로세싱)

---

**문서 작성 완료**: 2025년
**엔진 버전**: 1.0 (학습용)
**작성자**: Claude Code Assistant
