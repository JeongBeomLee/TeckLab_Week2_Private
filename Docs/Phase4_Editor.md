# Phase 4: Editor

## Overview
Editor 시스템은 Slate UI 프레임워크, 뷰포트 클라이언트, 에디터 도구로 구성됩니다. Unreal Engine의 Slate UI 시스템을 모방하여 선언적 UI 구축과 이벤트 기반 상호작용을 제공합니다.

## System Architecture

```
Editor
├── Slate UI Framework (선언적 UI 시스템)
│   ├── SWidget (위젯 베이스)
│   ├── SLeafWidget (리프 위젯)
│   │   ├── SButton
│   │   └── STextBlock
│   └── SPanel (컨테이너 위젯)
│       ├── SHorizontalBox
│       └── SVerticalBox
├── Viewport System (뷰포트 렌더링)
│   ├── FViewportClient (뷰포트 클라이언트 베이스)
│   ├── FEditorViewportClient (에디터 카메라)
│   └── SEditorViewport (뷰포트 위젯)
├── Level Editor (레벨 에디터)
│   ├── SLevelViewport (레벨 뷰포트)
│   └── SLevelViewportToolbar (툴바)
└── Event System (이벤트/델리게이트)
    ├── TDelegate (단일 델리게이트)
    └── TMulticastDelegate (멀티캐스트 델리게이트)
```

---

## 1. Slate UI Framework

### 1.1 SlateCore.h - 핵심 타입 정의

#### EVisibility (SlateCore.h:8)
**위젯 가시성 상태**

```cpp
enum class EVisibility : uint8
{
    Visible,    // 보이고 공간 차지
    Collapsed,  // 안 보이고 공간도 차지 안 함
    Hidden      // 안 보이지만 공간은 차지함
};
```

#### EHorizontalAlignment (SlateCore.h:15)
**수평 정렬**

```cpp
enum class EHorizontalAlignment : uint8
{
    HAlign_Fill,     // 전체 채우기
    HAlign_Left,     // 왼쪽 정렬
    HAlign_Center,   // 가운데 정렬
    HAlign_Right     // 오른쪽 정렬
};
```

#### EVerticalAlignment (SlateCore.h:23)
**수직 정렬**

```cpp
enum class EVerticalAlignment : uint8
{
    VAlign_Fill,     // 전체 채우기
    VAlign_Top,      // 위쪽 정렬
    VAlign_Center,   // 가운데 정렬
    VAlign_Bottom    // 아래쪽 정렬
};
```

---

### 1.2 FGeometry (SlateCore.h:31)
**위젯 기하학 정보 - Rendering Transform**

#### 역할
- 위젯의 화면상 위치와 크기
- 레이아웃 계산 결과
- 마우스 히트 테스트

#### 주요 멤버 변수
```cpp
FVector2 AbsolutePosition;              // 화면 절대 좌표
FVector2 LocalSize;                     // 위젯 크기
FVector2 AccumulatedRenderTransform;    // 누적 렌더 변환
float Scale;                            // 스케일
```

#### 핵심 메서드
```cpp
FVector2 GetLocalSize() const;
FVector2 GetAbsolutePosition() const;
float GetScale() const;

// 마우스 히트 테스트
bool IsUnderLocation(const FVector2& AbsoluteCoordinate) const;
```

---

### 1.3 FSlotBase / TSlot (SlateCore.h:67)
**슬롯 정보 - 자식 위젯 배치 정보**

#### FSlotBase (SlateCore.h:67)
**슬롯 베이스 클래스**

```cpp
struct FSlotBase
{
    EHorizontalAlignment HAlignment = EHorizontalAlignment::HAlign_Fill;
    EVerticalAlignment VAlignment = EVerticalAlignment::VAlign_Fill;
    FVector2 Padding = FVector2::Zero;

    // Fluent Interface (Method Chaining)
    FSlotBase& SetPadding(const FVector2& InPadding);
    FSlotBase& SetHAlign(EHorizontalAlignment InHAlignment);
    FSlotBase& SetVAlign(EVerticalAlignment InVAlignment);
};
```

#### TSlot<WidgetType> (SlateCore.h:78)
**제네릭 슬롯 템플릿**

```cpp
template<typename WidgetType>
struct TSlot : public FSlotBase
{
    TSharedPtr<WidgetType> Widget;

    TSlot& operator[](TSharedPtr<WidgetType> InWidget)
    {
        Widget = InWidget;
        return *this;
    }

    TSharedPtr<WidgetType> GetWidget() const;
};

using FSlot = TSlot<SWidget>;  // 기본 슬롯 타입
```

---

### 1.4 FPaintArgs (SlateCore.h:98)
**렌더링 인자 - Rendering Context**

#### 역할
- 위젯 렌더링에 필요한 정보 전달

#### 구조
```cpp
struct FPaintArgs
{
    FGeometry AllottedGeometry;  // 할당된 기하학 정보
    float DeltaTime;             // 프레임 간격
    bool bParentEnabled;         // 부모 활성화 상태
};
```

---

### 1.5 FPointerEvent (SlateCore.h:110)
**마우스 이벤트 정보**

#### 역할
- 마우스 입력 정보 전달

#### 주요 멤버 변수
```cpp
FVector2 ScreenSpacePosition;       // 현재 마우스 위치
FVector2 LastScreenSpacePosition;   // 이전 마우스 위치
FVector2 CursorDelta;               // 마우스 이동량
bool bIsLeftMouseButtonDown;        // 왼쪽 버튼
bool bIsRightMouseButtonDown;       // 오른쪽 버튼
bool bIsMiddleMouseButtonDown;      // 가운데 버튼
```

#### 핵심 메서드
```cpp
FVector2 GetScreenSpacePosition() const;
FVector2 GetCursorDelta() const;
bool IsMouseButtonDown(int32 Button) const;
```

---

### 1.6 SWidget (SWidget.h:4)
**위젯 베이스 클래스 - Abstract Base**

#### 역할
- 모든 Slate 위젯의 최상위 베이스
- 레이아웃 시스템 인터페이스
- 이벤트 처리 인터페이스

#### 상속 관계
```
SWidget (Abstract)
├── SLeafWidget (리프 위젯)
│   ├── SButton
│   └── STextBlock
└── SPanel (컨테이너 위젯)
    ├── SHorizontalBox
    └── SVerticalBox
```

#### 주요 멤버 변수
```cpp
EVisibility Visibility = EVisibility::Visible;
bool bEnabled = true;
bool bCanHaveChildren = false;
FVector2 DesiredSize = FVector2::Zero;
bool bNeedsDesiredSizeCompute = true;

// 캐싱
mutable FVector2 CachedDesiredSize = FVector2::Zero;
mutable bool bCachedDesiredSizeValid = false;
```

#### 핵심 메서드 (순수 가상 함수)
```cpp
// 레이아웃 시스템
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const = 0;
virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const;
virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const;

// 이벤트 처리
virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
virtual bool OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

// Tick
virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime);

// 레이아웃 무효화
virtual void OnArrangeChildren(const FGeometry& AllottedGeometry);
```

#### 상태 관리
```cpp
void SetVisibility(EVisibility InVisibility);
EVisibility GetVisibility() const;

void SetEnabled(bool bInEnabled);
bool IsEnabled() const;

bool CanHaveChildren() const;
```

#### 레이아웃 시스템 (Lazy Evaluation)
```cpp
FVector2 GetDesiredSize() const;
void SlatePrepass(float LayoutScaleMultiplier = 1.0f);
void InvalidateLayout();

protected:
    virtual void CacheDesiredSize(float LayoutScaleMultiplier);
```

#### RTTI
```cpp
static const FString& GetWidgetType();
virtual const FString& GetType() const;
```

---

### 1.7 SLeafWidget (SLeafWidget.h:5)
**리프 위젯 - 자식이 없는 위젯**

#### 역할
- 자식을 가질 수 없는 위젯의 베이스
- SButton, STextBlock 등의 기반

#### 상속 관계
```
SWidget
└── SLeafWidget
    ├── SButton
    └── STextBlock
```

#### 주요 멤버 변수
```cpp
FVector2 LeafDesiredSize = FVector2(100.0f, 30.0f);  // 기본 크기
```

#### 핵심 메서드
```cpp
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;

// ArrangeChildren은 final (리프는 자식 없음)
virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const final override {}

void SetDesiredSize(const FVector2& InDesiredSize);
FVector2 GetLeafDesiredSize() const;
```

---

### 1.8 SButton (SLeafWidget.h:26)
**버튼 위젯 - Interactive Widget**

#### 역할
- 클릭 가능한 버튼 UI
- 델리게이트를 통한 이벤트 전달

#### 상속 관계
```
SWidget → SLeafWidget → SButton
```

#### 주요 멤버 변수
```cpp
FString ButtonText = "Button";  // 버튼 텍스트
bool bIsPressed = false;        // 눌림 상태
bool bIsHovered = false;        // 호버 상태
```

#### 핵심 메서드
```cpp
// 레이아웃
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;

// 마우스 이벤트
virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

// 버튼 상태
void SetText(const FString& InText);
FString GetText() const;
bool IsPressed() const;
bool IsHovered() const;

protected:
    void ExecuteOnClick();
```

#### 델리게이트 (SLeafWidget.h:55)
```cpp
DECLARE_DELEGATE(FOnClicked)
FOnClicked OnClicked;  // 클릭 이벤트
```

#### 사용 예시
```cpp
TSharedPtr<SButton> Button = MakeShared<SButton>();
Button->SetText("Click Me");
Button->OnClicked.BindLambda([]() {
    // 버튼 클릭 시 실행
});
```

---

### 1.9 STextBlock (SLeafWidget.h:62)
**텍스트 위젯 - Display Widget**

#### 역할
- 텍스트 표시
- 폰트 크기 조절

#### 상속 관계
```
SWidget → SLeafWidget → STextBlock
```

#### 주요 멤버 변수
```cpp
FString Text = "Text";      // 표시할 텍스트
float FontSize = 12.0f;     // 폰트 크기
```

#### 핵심 메서드
```cpp
// 레이아웃
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;

// 텍스트 설정
void SetText(const FString& InText);
FString GetText() const;

// 폰트 설정
void SetFontSize(float InFontSize);
float GetFontSize() const;

private:
    FVector2 CalculateTextSize() const;
```

---

### 1.10 SPanel (SPanel.h:4)
**패널 위젯 - 컨테이너 베이스**

#### 역할
- 자식 위젯들을 담는 컨테이너
- 레이아웃 계산 및 배치

#### 상속 관계
```
SWidget
└── SPanel
    ├── SHorizontalBox
    └── SVerticalBox
```

#### 주요 멤버 변수
```cpp
TArray<FSlot> Children;  // 자식 위젯 슬롯들
```

#### 핵심 메서드
```cpp
// 레이아웃 (하위 클래스에서 구현)
virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<FSlot>& ArrangedChildren) const override;
virtual void OnArrangeChildren(const FGeometry& AllottedGeometry) override;

// 자식 관리
int32 GetChildrenCount() const;
TArray<FSlot>& GetChildren();
const TArray<FSlot>& GetChildren() const;

FSlot& AddSlot();
bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
void ClearChildren();

protected:
    void InvalidateChildOrder();
```

---

### 1.11 SHorizontalBox (SPanel.h:32)
**수평 박스 - 가로 배치 컨테이너**

#### 역할
- 자식들을 가로로 배치
- Fill/Auto Width 지원

#### 상속 관계
```
SWidget → SPanel → SHorizontalBox
```

#### FSlot (SHorizontalBox::FSlot) (SPanel.h:35)
**수평 박스 슬롯**

```cpp
struct FSlot : public FSlotBase
{
    TSharedPtr<SWidget> Widget;
    float FillWidth = 0.0f;     // 채우기 비율
    float MaxWidth = 0.0f;      // 최대 너비
    bool bAutoWidth = true;     // 자동 너비

    FSlot& SetFillWidth(float InFillWidth);
    FSlot& SetMaxWidth(float InMaxWidth);
    FSlot& SetAutoWidth(bool bInAutoWidth);
};
```

#### 주요 멤버 변수
```cpp
TArray<FSlot> HorizontalSlots;  // 수평 슬롯들
```

#### 핵심 메서드
```cpp
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const override;

FSlot& AddSlot();
bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
void ClearSlots();

int32 GetSlotCount() const;
const TArray<FSlot>& GetSlots() const;
```

#### 레이아웃 계산
```
1. Auto Width 슬롯: ComputeDesiredSize()로 너비 계산
2. Fill Width 슬롯: 남은 공간을 FillWidth 비율로 분배
3. MaxWidth 제한 적용
```

---

### 1.12 SVerticalBox (SPanel.h:71)
**수직 박스 - 세로 배치 컨테이너**

#### 역할
- 자식들을 세로로 배치
- Fill/Auto Height 지원

#### 상속 관계
```
SWidget → SPanel → SVerticalBox
```

#### FSlot (SVerticalBox::FSlot) (SPanel.h:74)
**수직 박스 슬롯**

```cpp
struct FSlot : public FSlotBase
{
    TSharedPtr<SWidget> Widget;
    float FillHeight = 0.0f;    // 채우기 비율
    float MaxHeight = 0.0f;     // 최대 높이
    bool bAutoHeight = true;    // 자동 높이

    FSlot& SetFillHeight(float InFillHeight);
    FSlot& SetMaxHeight(float InMaxHeight);
    FSlot& SetAutoHeight(bool bInAutoHeight);
};
```

#### 주요 멤버 변수
```cpp
TArray<FSlot> VerticalSlots;  // 수직 슬롯들
```

#### 핵심 메서드
```cpp
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual void ArrangeChildren(const FGeometry& AllottedGeometry, TArray<::FSlot>& ArrangedChildren) const override;

FSlot& AddSlot();
bool RemoveSlot(TSharedPtr<SWidget> SlotWidget);
void ClearSlots();

int32 GetSlotCount() const;
const TArray<FSlot>& GetSlots() const;
```

---

## 2. Viewport System

### 2.1 FViewportClient (ViewportClient.h:36)
**뷰포트 클라이언트 베이스 클래스**

#### 역할
- 뷰포트 렌더링 로직
- 입력 처리
- 씬뷰 관리

#### 상속 관계
```
FViewportClient
└── FEditorViewportClient
```

#### EViewportType (ViewportClient.h:9)
```cpp
enum class EViewportType
{
    Game,       // 게임 뷰포트
    Editor,     // 에디터 뷰포트
    Preview     // 프리뷰 뷰포트
};
```

#### EMouseButton (ViewportClient.h:17)
```cpp
enum class EMouseButton
{
    Left,
    Right,
    Middle
};
```

#### FViewportInputEvent (ViewportClient.h:23)
**뷰포트 입력 이벤트**

```cpp
struct FViewportInputEvent
{
    int32 MouseX = 0;
    int32 MouseY = 0;
    int32 MouseDeltaX = 0;
    int32 MouseDeltaY = 0;
    EMouseButton Button = EMouseButton::Left;
    bool bIsPressed = false;
    bool bCtrlPressed = false;
    bool bShiftPressed = false;
    bool bAltPressed = false;
};
```

#### 주요 멤버 변수
```cpp
FSceneView* SceneView = nullptr;                // 씬 뷰
EViewportType ViewportType = EViewportType::Game;
uint32 ViewportWidth = 1920;
uint32 ViewportHeight = 1080;
bool bIsActive = false;
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void Initialize(uint32 Width, uint32 Height);
virtual void Shutdown();

// 렌더링
virtual void Draw(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer);
virtual void Tick(float DeltaTime);

// 입력 이벤트 (하위 클래스에서 구현)
virtual void OnMouseMove(const FViewportInputEvent& Event);
virtual void OnMouseButtonDown(const FViewportInputEvent& Event);
virtual void OnMouseButtonUp(const FViewportInputEvent& Event);
virtual void OnKeyDown(uint32 KeyCode);
virtual void OnKeyUp(uint32 KeyCode);

// 뷰 설정
virtual void SetViewLocation(const FVector& NewLocation);
virtual void SetViewRotation(const FVector& NewRotation);
virtual void SetProjectionType(EProjectionType NewType);
virtual void SetFOV(float NewFOV);

// 접근자
FSceneView* GetSceneView() const;
EViewportType GetViewportType() const;
uint32 GetViewportWidth() const;
uint32 GetViewportHeight() const;
bool IsActive() const;

void SetActive(bool bActive);
void SetViewportSize(uint32 Width, uint32 Height);

protected:
    virtual void CreateSceneView();
    virtual void UpdateSceneView(float DeltaTime);
```

---

### 2.2 FEditorViewportClient (EditorViewportClient.h:19)
**에디터 뷰포트 클라이언트 - 카메라 컨트롤**

#### 역할
- 에디터 카메라 조작
- 뷰 모드 전환
- 오빗/1인칭/자유 카메라

#### 상속 관계
```
FViewportClient
└── FEditorViewportClient
```

#### EEditorViewMode (EditorViewportClient.h:4)
**에디터 뷰 모드**

```cpp
enum class EEditorViewMode
{
    Wireframe,      // 와이어프레임
    Unlit,          // 라이팅 없음
    Lit,            // 라이팅 적용
    VertexColor     // 정점 색상
};
```

#### ECameraMode (EditorViewportClient.h:12)
**카메라 모드**

```cpp
enum class ECameraMode
{
    Orbit,          // 오빗 카메라 (타겟 중심 회전)
    FirstPerson,    // 1인칭 카메라
    Free            // 자유 카메라
};
```

#### 주요 멤버 변수
```cpp
// 뷰 모드
EEditorViewMode ViewMode = EEditorViewMode::Lit;
ECameraMode CameraMode = ECameraMode::Orbit;

// 오빗 카메라 설정
FVector OrbitTarget = FVector::Zero;    // 회전 중심점
float OrbitDistance = 500.0f;           // 타겟으로부터의 거리
float OrbitPitch = -15.0f;              // 피치 각도
float OrbitYaw = 0.0f;                  // 요 각도

// 카메라 설정
float CameraSpeed = 300.0f;             // 이동 속도
float MouseSensitivity = 1.0f;          // 마우스 민감도

// 마우스 캡처
bool bMouseCaptured = false;
FVector LastMousePosition = FVector::Zero;
```

#### 핵심 메서드
```cpp
// 생명주기
virtual void Initialize(uint32 Width, uint32 Height) override;
virtual void Tick(float DeltaTime) override;

// 입력 처리
virtual void OnMouseMove(const FViewportInputEvent& Event) override;
virtual void OnMouseButtonDown(const FViewportInputEvent& Event) override;
virtual void OnMouseButtonUp(const FViewportInputEvent& Event) override;
virtual void OnKeyDown(uint32 KeyCode) override;
virtual void OnKeyUp(uint32 KeyCode) override;

// 뷰/카메라 설정
void SetViewMode(EEditorViewMode NewMode);
void SetCameraMode(ECameraMode NewMode);
void SetOrbitTarget(const FVector& NewTarget);
void SetOrbitDistance(float NewDistance);
void SetCameraSpeed(float NewSpeed);
void SetMouseSensitivity(float NewSensitivity);

// 접근자
EEditorViewMode GetViewMode() const;
ECameraMode GetCameraMode() const;
const FVector& GetOrbitTarget() const;
float GetOrbitDistance() const;

// 카메라 조작
void FocusOnPoint(const FVector& Point);
void ResetCamera();

protected:
    virtual void CreateSceneView() override;
    virtual void UpdateSceneView(float DeltaTime) override;

private:
    // 카메라 모드별 업데이트
    void UpdateOrbitCamera();
    void UpdateFirstPersonCamera();
    void UpdateFreeCamera();

    // 입력 처리
    void ProcessCameraInput(float DeltaTime);
    void ProcessMouseInput(const FViewportInputEvent& Event);

    // 유틸리티
    FVector CalculateOrbitPosition() const;
```

#### 오빗 카메라 계산
```cpp
FVector CalculateOrbitPosition() const
{
    // 구면 좌표계 → 직교 좌표계 변환
    float RadPitch = FMath::DegreesToRadians(OrbitPitch);
    float RadYaw = FMath::DegreesToRadians(OrbitYaw);

    FVector Offset;
    Offset.X = OrbitDistance * cos(RadPitch) * cos(RadYaw);
    Offset.Y = OrbitDistance * cos(RadPitch) * sin(RadYaw);
    Offset.Z = OrbitDistance * sin(RadPitch);

    return OrbitTarget + Offset;
}
```

---

### 2.3 SEditorViewport (SEditorViewport.h:9)
**에디터 뷰포트 위젯 - UI + 렌더링 통합**

#### 역할
- 뷰포트 UI 위젯
- ViewportClient와 연결
- 렌더링 및 입력 이벤트 중계

#### 상속 관계
```
SWidget → SLeafWidget → SEditorViewport
```

#### 주요 멤버 변수
```cpp
TSharedPtr<FEditorViewportClient> ViewportClient;  // 뷰포트 클라이언트
FD3D11GraphicsDevice* GraphicsDevice = nullptr;    // 그래픽 디바이스
URenderer* Renderer = nullptr;                     // 렌더러

bool bIsRealtime = true;        // 실시간 렌더링
bool bShowStats = false;        // 통계 표시
float LastRenderTime = 0.0f;   // 마지막 렌더 시간
```

#### 핵심 메서드
```cpp
// SWidget 오버라이드
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, float InDeltaTime) const override;
virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime) override;

// 마우스 이벤트
virtual bool OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
virtual bool OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
virtual bool OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
virtual bool OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

// 초기화
void Initialize(FD3D11GraphicsDevice* InGraphicsDevice, URenderer* InRenderer);
void Shutdown();

// 뷰포트 클라이언트
void SetViewportClient(TSharedPtr<FEditorViewportClient> InViewportClient);
TSharedPtr<FEditorViewportClient> GetViewportClient() const;

// 설정
void SetRealtime(bool bInRealtime);
bool IsRealtime() const;

void SetShowStats(bool bInShowStats);
bool ShouldShowStats() const;

float GetLastRenderTime() const;

protected:
    void RenderViewport(const FGeometry& AllottedGeometry, float DeltaTime);
    void UpdateViewportClient(const FGeometry& AllottedGeometry, float DeltaTime);

private:
    FPointerEvent ConvertToViewportEvent(const FGeometry& MyGeometry, const FPointerEvent& InMouseEvent) const;
```

---

## 3. Level Editor

### 3.1 SLevelViewportToolbar (SLevelViewport.h:17)
**레벨 뷰포트 툴바**

#### 역할
- 뷰포트 컨트롤 버튼
- 통계 표시
- 뷰 모드/카메라 모드 전환

#### 상속 관계
```
SWidget → SPanel → SHorizontalBox → SLevelViewportToolbar
```

#### 주요 멤버 변수
```cpp
TSharedPtr<SButton> ViewModeButton;         // 뷰 모드 버튼
TSharedPtr<SButton> CameraModeButton;       // 카메라 모드 버튼
TSharedPtr<SButton> RealTimeButton;         // 실시간 버튼
TSharedPtr<STextBlock> ViewportStatsText;   // 통계 텍스트
```

#### 핵심 메서드
```cpp
void Initialize();
void SetViewportType(ELevelViewportType InType);
void SetRealTimeEnabled(bool bEnabled);
void UpdateStats(float FrameTime, int32 ActorCount);

FString GetViewModeText(ELevelViewportType Type) const;

private:
    void OnViewModeClicked();
    void OnCameraModeClicked();
    void OnRealTimeClicked();
```

---

### 3.2 SLevelViewport (SLevelViewport.h:47)
**레벨 뷰포트 - 통합 레벨 에디터 UI**

#### 역할
- 레벨 편집 뷰포트
- 툴바 + 에디터 뷰포트 통합
- 레벨/월드 연결

#### 상속 관계
```
SWidget → SPanel → SVerticalBox → SLevelViewport
```

#### ELevelViewportType (SLevelViewport.h:9)
**레벨 뷰포트 타입**

```cpp
enum class ELevelViewportType
{
    Perspective,            // 원근 투영
    Orthographic_Top,       // 직교 투영 (위)
    Orthographic_Side,      // 직교 투영 (옆)
    Orthographic_Front      // 직교 투영 (앞)
};
```

#### 주요 멤버 변수
```cpp
TSharedPtr<SLevelViewportToolbar> ViewportToolbar;  // 툴바
TSharedPtr<SEditorViewport> EditorViewport;         // 에디터 뷰포트

ELevelViewportType ViewportType = ELevelViewportType::Perspective;
ULevel* CurrentLevel = nullptr;     // 현재 레벨
UWorld* CurrentWorld = nullptr;     // 현재 월드

bool bShowToolbar = true;           // 툴바 표시
bool bShowGrid = true;              // 그리드 표시
bool bShowActorInfo = false;        // 액터 정보 표시
```

#### 핵심 메서드
```cpp
// SWidget 오버라이드
virtual FVector2 ComputeDesiredSize(float LayoutScaleMultiplier = 1.0f) const override;
virtual void Tick(const FGeometry& AllottedGeometry, float InCurrentTime, float InDeltaTime) override;

// 초기화
void Initialize(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer);
void Shutdown();

// 레벨/월드 설정
void SetLevel(ULevel* InLevel);
void SetWorld(UWorld* InWorld);
void SetViewportType(ELevelViewportType InType);

// 접근자
ULevel* GetLevel() const;
UWorld* GetWorld() const;
ELevelViewportType GetViewportType() const;

TSharedPtr<SEditorViewport> GetEditorViewport() const;
TSharedPtr<SLevelViewportToolbar> GetToolbar() const;

// 설정
void SetShowToolbar(bool bShow);
void SetShowGrid(bool bShow);
void SetShowActorInfo(bool bShow);

bool ShouldShowToolbar() const;
bool ShouldShowGrid() const;
bool ShouldShowActorInfo() const;

// 카메라 조작
void FocusOnSelection();
void ResetCamera();

protected:
    void CreateViewportToolbar();
    void CreateEditorViewport();
    void UpdateViewportForLevel();
    void UpdateToolbarStats();

private:
    void OnViewportTypeChanged();
    void ConfigureViewportForType(ELevelViewportType Type);
```

---

## 4. Event System

### 4.1 TDelegate<Args...> (Delegate.h:5)
**단일 델리게이트 - Event Handler**

#### 역할
- 함수 포인터 래퍼
- 람다, 멤버 함수, UObject 메서드 바인딩
- 콜백 시스템

#### 주요 멤버 변수
```cpp
std::function<void(Args...)> Function;  // 저장된 함수
```

#### 핵심 메서드
```cpp
// 람다 바인딩
template<typename T>
void BindLambda(T&& Lambda);

// Raw 멤버 함수 바인딩
template<typename T, typename Method>
void BindRaw(T* Object, Method&& method);

// UObject 멤버 함수 바인딩 (유효성 체크 포함)
template<typename T, typename Method>
void BindUObject(T* Object, Method&& method);

// 실행
void Execute(Args... args);
void ExecuteIfBound(Args... args);

// 상태 확인
bool IsBound() const;
void Unbind();
```

#### 사용 예시
```cpp
// 람다 바인딩
TDelegate<int, float> MyDelegate;
MyDelegate.BindLambda([](int x, float y) {
    // 처리
});
MyDelegate.Execute(10, 3.14f);

// 멤버 함수 바인딩
class MyClass {
    void OnEvent(int x) { /* ... */ }
};
MyClass Instance;
TDelegate<int> Delegate;
Delegate.BindRaw(&Instance, &MyClass::OnEvent);
Delegate.Execute(42);
```

---

### 4.2 TMulticastDelegate<Args...> (Delegate.h:64)
**멀티캐스트 델리게이트 - Multiple Handlers**

#### 역할
- 여러 함수를 등록하여 동시 호출
- 이벤트 시스템 (Observer Pattern)

#### 주요 멤버 변수
```cpp
std::vector<std::function<void(Args...)>> Functions;  // 등록된 함수들
```

#### 핵심 메서드
```cpp
// 람다 추가
template<typename T>
void AddLambda(T&& Lambda);

// Raw 멤버 함수 추가
template<typename T, typename Method>
void AddRaw(T* Object, Method&& method);

// UObject 멤버 함수 추가
template<typename T, typename Method>
void AddUObject(T* Object, Method&& method);

// 모든 함수 호출
void Broadcast(Args... args);

// 관리
void Clear();
bool IsBound() const;
size_t GetBoundFunctionCount() const;
```

#### 사용 예시
```cpp
TMulticastDelegate<FString> OnLevelLoaded;

// 여러 핸들러 등록
OnLevelLoaded.AddLambda([](FString LevelName) {
    // 로딩 UI 업데이트
});
OnLevelLoaded.AddLambda([](FString LevelName) {
    // 통계 기록
});

// 모든 핸들러 호출
OnLevelLoaded.Broadcast("MyLevel");
```

---

### 4.3 델리게이트 매크로 (Delegate.h:124-147)

#### 단일 델리게이트
```cpp
#define DECLARE_DELEGATE(DelegateName)
    using DelegateName = TDelegate<>;

#define DECLARE_DELEGATE_OneParam(DelegateName, Param1Type)
    using DelegateName = TDelegate<Param1Type>;

#define DECLARE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type)
    using DelegateName = TDelegate<Param1Type, Param2Type>;

#define DECLARE_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type)
    using DelegateName = TDelegate<Param1Type, Param2Type, Param3Type>;
```

#### 멀티캐스트 델리게이트
```cpp
#define DECLARE_MULTICAST_DELEGATE(DelegateName)
    using DelegateName = TMulticastDelegate<>;

#define DECLARE_MULTICAST_DELEGATE_OneParam(DelegateName, Param1Type)
    using DelegateName = TMulticastDelegate<Param1Type>;

#define DECLARE_MULTICAST_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type)
    using DelegateName = TMulticastDelegate<Param1Type, Param2Type>;

#define DECLARE_MULTICAST_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type)
    using DelegateName = TMulticastDelegate<Param1Type, Param2Type, Param3Type>;
```

#### 사용 예시
```cpp
// 선언
DECLARE_DELEGATE_OneParam(FOnButtonClicked, int32)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, ULevel*, ULevel*)

// 사용
FOnButtonClicked OnClicked;
OnClicked.BindLambda([](int32 ButtonID) { /* ... */ });

FOnLevelChanged OnLevelChanged;
OnLevelChanged.AddLambda([](ULevel* OldLevel, ULevel* NewLevel) { /* ... */ });
OnLevelChanged.Broadcast(OldLevel, NewLevel);
```

---

## Key Flows

### Slate UI 레이아웃 및 렌더링 흐름

```
1. SlatePrepass (레이아웃 계산)
   Widget->SlatePrepass(LayoutScaleMultiplier)
   ├─ if (bNeedsDesiredSizeCompute)
   │   ├─ CacheDesiredSize(LayoutScaleMultiplier)
   │   │   └─ CachedDesiredSize = ComputeDesiredSize(LayoutScaleMultiplier)
   │   └─ bNeedsDesiredSizeCompute = false
   └─ for (Child : Children)
       └─ Child->SlatePrepass(LayoutScaleMultiplier)  // 재귀

2. ArrangeChildren (자식 배치)
   Panel->ArrangeChildren(AllottedGeometry, ArrangedChildren)
   ├─ for (Slot : HorizontalSlots)
   │   {
   │       // 자식 위젯 위치/크기 계산
   │       FGeometry ChildGeometry = ...;
   │       ArrangedChildren.push_back(FSlot(Slot.Widget, ChildGeometry));
   │   }
   └─ return ArrangedChildren

3. OnPaint (렌더링)
   Widget->OnPaint(PaintArgs, AllottedGeometry, DeltaTime)
   ├─ if (Visibility == Collapsed) return 0;
   ├─ // 자신 렌더링
   ├─ DrawBackground(AllottedGeometry);
   ├─ DrawContent(AllottedGeometry);
   ├─ // 자식 렌더링
   └─ for (Child : Children)
       └─ Child->OnPaint(PaintArgs, ChildGeometry, DeltaTime)  // 재귀
```

### 마우스 이벤트 전파 흐름 (Bubbling)

```
User Mouse Click
    ↓
Windows Message (WM_LBUTTONDOWN)
    ↓
SLevelViewport->OnMouseButtonDown(Geometry, PointerEvent)
    ↓
SEditorViewport->OnMouseButtonDown(Geometry, PointerEvent)
    ├─ ConvertToViewportEvent() (좌표 변환)
    └─ ViewportClient->OnMouseButtonDown(ViewportInputEvent)
        ├─ FEditorViewportClient::OnMouseButtonDown()
        │   ├─ bMouseCaptured = true (if RightButton)
        │   └─ LastMousePosition = Event.Position
        │
        └─ (이벤트 처리 완료 또는 버블링 계속)
```

### 뷰포트 렌더링 흐름

```
Application Loop
    ↓
SLevelViewport->Tick(Geometry, CurrentTime, DeltaTime)
    ├─ ViewportToolbar->Tick()
    │   └─ UpdateStats(FrameTime, ActorCount)
    │
    └─ EditorViewport->Tick(Geometry, CurrentTime, DeltaTime)
        ├─ UpdateViewportClient(Geometry, DeltaTime)
        │   └─ ViewportClient->Tick(DeltaTime)
        │       └─ UpdateSceneView(DeltaTime)
        │           ├─ UpdateOrbitCamera() / UpdateFreeCamera()
        │           └─ SceneView->SetViewLocation/Rotation()
        │
        └─ RenderViewport(Geometry, DeltaTime)
            └─ ViewportClient->Draw(GraphicsDevice, Renderer)
                ├─ Renderer->RenderSceneWithView(SceneView)
                │   ├─ FRenderPassContext 생성
                │   ├─ DepthPrePass->Execute()
                │   └─ BasePass->Execute()
                │       └─ World->GetActorsOfClass<>()로 렌더 가능한 액터 수집
                │           └─ StaticMeshComponent->Render()
                └─ GraphicsDevice->SwapBuffer()
```

### 에디터 카메라 조작 흐름 (Orbit Mode)

```
User Input: Right Mouse Button Drag
    ↓
FEditorViewportClient::OnMouseMove(Event)
    ├─ if (bMouseCaptured && Event.bIsRightMouseButtonDown)
    │   {
    │       ProcessMouseInput(Event)
    │       ├─ OrbitYaw += Event.MouseDeltaX * MouseSensitivity
    │       └─ OrbitPitch += Event.MouseDeltaY * MouseSensitivity
    │   }
    └─ UpdateSceneView(DeltaTime)
        └─ UpdateOrbitCamera()
            ├─ FVector CameraPosition = CalculateOrbitPosition()
            │   └─ // 구면 좌표 → 직교 좌표 변환
            │       Position = OrbitTarget + Spherical(OrbitDistance, OrbitPitch, OrbitYaw)
            │
            ├─ SceneView->SetViewLocation(CameraPosition)
            └─ SceneView->SetViewRotation(LookAt(CameraPosition, OrbitTarget))
```

### 델리게이트 이벤트 흐름

```
1. 버튼 클릭 이벤트
   SButton::OnMouseButtonUp(Geometry, PointerEvent)
   ├─ if (Geometry.IsUnderLocation(PointerEvent.GetScreenSpacePosition()))
   │   {
   │       bIsPressed = false;
   │       ExecuteOnClick();
   │   }
   └─ ExecuteOnClick()
       └─ if (OnClicked.IsBound())
           OnClicked.Execute();  // 델리게이트 실행
               └─ // 바인딩된 람다/함수 호출

2. 멀티캐스트 델리게이트 (레벨 로드 이벤트)
   ULevel::LoadLevel()
   ├─ // 레벨 로딩 작업
   └─ OnLevelLoaded.Broadcast(this);
       └─ for (Function : Functions)
           Function(this);  // 모든 핸들러 호출
               ├─ Handler1(Level)
               ├─ Handler2(Level)
               └─ Handler3(Level)
```

---

## Design Patterns Summary

### 1. Composite Pattern
- **SWidget → SPanel → Children**: UI 계층 구조
- **SHorizontalBox/SVerticalBox**: 자식 위젯 재귀 배치

### 2. Template Method Pattern
- **SWidget**: ComputeDesiredSize, ArrangeChildren, OnPaint 가상 함수
- **하위 클래스**: 각자의 방식으로 구현

### 3. Strategy Pattern
- **ECameraMode**: Orbit/FirstPerson/Free 카메라 전략
- **EEditorViewMode**: Wireframe/Unlit/Lit 렌더링 전략

### 4. Observer Pattern
- **TMulticastDelegate**: 이벤트 발생 시 모든 핸들러 호출
- **OnClicked, OnLevelLoaded** 등

### 5. Lazy Evaluation Pattern
- **SWidget::CachedDesiredSize**: 필요할 때만 레이아웃 재계산
- **bNeedsDesiredSizeCompute**: Dirty Flag

### 6. Fluent Interface Pattern
- **FSlotBase**: SetPadding().SetHAlign().SetVAlign() 체이닝
- **SHorizontalBox::FSlot**: SetFillWidth().SetMaxWidth()

### 7. Facade Pattern
- **SLevelViewport**: Toolbar + EditorViewport 통합
- **복잡한 레벨 편집 기능을 단순 인터페이스로 제공**

### 8. Adapter Pattern
- **SEditorViewport**: FPointerEvent → FViewportInputEvent 변환
- **ConvertToViewportEvent()**

### 9. Command Pattern
- **TDelegate**: 함수/람다를 객체로 캡슐화
- **BindLambda, BindRaw, BindUObject**

### 10. Mediator Pattern
- **FEditorViewportClient**: 입력 → 카메라 → SceneView 중재

---

## Class Hierarchy Diagram

```
┌────────────────────────────────────────────────────────────┐
│                        Editor                               │
└────────────────────────────────────────────────────────────┘
                          │
        ┌─────────────────┼────────────────────┬─────────────┐
        │                 │                    │             │
        ▼                 ▼                    ▼             ▼
  ┌──────────┐      ┌──────────┐      ┌──────────┐  ┌──────────┐
  │ Slate UI │      │ Viewport │      │  Level   │  │  Event   │
  └──────────┘      └──────────┘      └──────────┘  └──────────┘
        │                 │                  │             │
        ▼                 ▼                  ▼             ▼
    SWidget        FViewportClient    SLevelViewport  TDelegate
        │                 │                  │             │
   ┌────┴────┐            ▼                  ▼             ▼
   ▼         ▼     FEditorViewport    SEditorViewport  TMulticast
SLeafWidget SPanel      Client                            Delegate
   │         │
   ▼         ▼
SButton  SHorizontalBox
STextBlock SVerticalBox
```

### 상세 클래스 계층

```
Slate UI
└── SWidget (Abstract)
    ├── SLeafWidget
    │   ├── SButton (델리게이트: OnClicked)
    │   ├── STextBlock
    │   └── SEditorViewport
    │
    └── SPanel
        ├── SHorizontalBox (FSlot: FillWidth, AutoWidth)
        │   └── SLevelViewportToolbar
        └── SVerticalBox (FSlot: FillHeight, AutoHeight)
            └── SLevelViewport

Viewport System
└── FViewportClient
    ├── FSceneView*
    ├── EViewportType
    └── FEditorViewportClient
        ├── ECameraMode (Orbit/FirstPerson/Free)
        ├── EEditorViewMode (Wireframe/Unlit/Lit)
        └── 오빗 카메라 파라미터

Level Editor
└── SLevelViewport : SVerticalBox
    ├── SLevelViewportToolbar* (툴바)
    ├── SEditorViewport* (뷰포트)
    ├── ULevel* (현재 레벨)
    └── UWorld* (현재 월드)

Event System
├── TDelegate<Args...>
│   ├── BindLambda()
│   ├── BindRaw()
│   ├── BindUObject()
│   └── Execute()
│
└── TMulticastDelegate<Args...>
    ├── AddLambda()
    ├── AddRaw()
    ├── AddUObject()
    └── Broadcast()
```

---

## 다른 시스템과의 관계

### Editor → Core Systems
- SWidget은 UObject 계층과 무관 (독립적)
- FViewportClient는 UObject 상속하지 않음
- 델리게이트가 UObject 메서드 바인딩 지원 (BindUObject)

### Editor → Rendering Pipeline
- FViewportClient가 FSceneView 생성/관리
- SEditorViewport가 URenderer 호출
- FEditorViewportClient가 카메라 트랜스폼 제어

### Editor → Engine Core
- SLevelViewport가 UWorld/ULevel 참조
- 에디터가 액터 스폰/선택/편집
- 뷰포트에서 씬 렌더링

---

## 참고: 클래스 매핑

| 분류 | 클래스/구조체 | 주요 역할 |
|-----|-------------|---------|
| **Slate Core** | SWidget | 위젯 베이스 |
| **Slate Core** | SLeafWidget | 리프 위젯 베이스 |
| **Slate Core** | SButton | 버튼 위젯 |
| **Slate Core** | STextBlock | 텍스트 위젯 |
| **Slate Core** | SPanel | 패널 베이스 |
| **Slate Core** | SHorizontalBox | 수평 박스 |
| **Slate Core** | SVerticalBox | 수직 박스 |
| **Slate Core** | FGeometry | 위젯 기하학 |
| **Slate Core** | FPaintArgs | 렌더링 인자 |
| **Slate Core** | FPointerEvent | 마우스 이벤트 |
| **Viewport** | FViewportClient | 뷰포트 클라이언트 베이스 |
| **Viewport** | FEditorViewportClient | 에디터 뷰포트 클라이언트 |
| **Viewport** | SEditorViewport | 에디터 뷰포트 위젯 |
| **Level Editor** | SLevelViewport | 레벨 뷰포트 |
| **Level Editor** | SLevelViewportToolbar | 레벨 뷰포트 툴바 |
| **Event** | TDelegate | 단일 델리게이트 |
| **Event** | TMulticastDelegate | 멀티캐스트 델리게이트 |

---

## 학습 체크리스트

- [ ] Slate UI 레이아웃 시스템 이해 (ComputeDesiredSize, ArrangeChildren)
- [ ] Lazy Evaluation 패턴 이해 (CachedDesiredSize, Dirty Flag)
- [ ] Composite Pattern으로 UI 계층 구조 이해
- [ ] 마우스 이벤트 버블링 메커니즘 이해
- [ ] 델리게이트 시스템 이해 (BindLambda, BindRaw, BindUObject)
- [ ] 멀티캐스트 델리게이트와 Observer Pattern 이해
- [ ] 에디터 카메라 모드 이해 (Orbit/FirstPerson/Free)
- [ ] 오빗 카메라 수학 이해 (구면 좌표 → 직교 좌표)
- [ ] ViewportClient와 SceneView 관계 이해
- [ ] SLevelViewport 통합 구조 이해

---

**완료**: 모든 Phase 분석 완료
**다음 단계**: 전체 시스템 통합 문서 작성 (선택 사항)
