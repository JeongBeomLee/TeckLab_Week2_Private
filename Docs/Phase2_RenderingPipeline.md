# Phase 2: Rendering Pipeline

## Overview
Rendering Pipeline은 DirectX11을 기반으로 한 그래픽 렌더링 시스템입니다. RHI(Rendering Hardware Interface) 추상화, 렌더러, 머티리얼 시스템, 씬 관리로 구성됩니다.

## System Architecture

```
Rendering Pipeline
├── RHI Layer (DirectX11 추상화)
│   └── FD3D11GraphicsDevice
├── Renderer Architecture
│   ├── URenderer (렌더 오케스트레이터)
│   ├── IRenderPass (렌더 패스 인터페이스)
│   ├── FDepthPrePass
│   └── FBasePass
├── Scene Management
│   ├── FSceneView (카메라/뷰포트)
│   └── FRenderPassContext
├── Material System
│   ├── UMaterialInterface (PBR 머티리얼)
│   └── UMaterialInstanceInterface
└── Mesh System
    ├── UStaticMesh (메시 에셋)
    ├── FStaticMeshRenderData (렌더 데이터)
    ├── FVertex (정점 구조)
    └── UKismetProceduralMeshLibrary (프로시저럴 메시)
```

---

## 1. RHI Layer (Rendering Hardware Interface)

### 1.1 FD3D11GraphicsDevice (D3D11GraphicsDevice.h:5)
**DirectX11 래퍼 클래스 - RHI 추상화**

#### 역할
- DirectX11 디바이스/컨텍스트 생성 및 관리
- 스왑체인, 렌더타겟, 깊이버퍼 관리
- 셰이더, 버퍼 생성 헬퍼
- 뷰포트 설정

#### 주요 멤버 변수
```cpp
// DirectX11 핵심 객체
ComPtr<ID3D11Device> Device;                    // GPU 디바이스
ComPtr<ID3D11DeviceContext> Context;            // 즉시 컨텍스트
ComPtr<IDXGISwapChain> SwapChain;               // 스왑체인

// 렌더 타겟
ComPtr<ID3D11RenderTargetView> BackBufferRTV;   // 백버퍼 RTV
ComPtr<ID3D11Texture2D> DepthStencilBuffer;     // 깊이 스텐실 버퍼
ComPtr<ID3D11DepthStencilView> DepthStencilView; // DSV

// 뷰포트
D3D11_VIEWPORT MainViewport;

// 디바이스 설정
FDeviceDesc DeviceDesc;
bool bInitialized;
```

#### FDeviceDesc (D3D11GraphicsDevice.h:8)
**디바이스 초기화 설정**

```cpp
struct FDeviceDesc
{
    HWND WindowHandle = nullptr;                           // 윈도우 핸들
    uint32 WindowWidth = 1920;                             // 백버퍼 너비
    uint32 WindowHeight = 1080;                            // 백버퍼 높이
    bool bFullscreen = false;                              // 전체화면 여부
    bool bVSync = true;                                    // 수직동기화
    DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;    // 백버퍼 포맷
    DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이 포맷
    uint32 MSAASamples = 1;                                // MSAA 샘플 수
    uint32 MSAAQuality = 0;                                // MSAA 품질
};
```

#### 핵심 메서드
```cpp
// 초기화/종료
bool Init(const FDeviceDesc& InDesc);
void Shutdown();

// 디바이스 접근
ID3D11Device* GetDevice() const;
ID3D11DeviceContext* GetContext() const;
IDXGISwapChain* GetSwapChain() const;

// 렌더 타겟 접근
ID3D11RenderTargetView* GetBackBufferRTV() const;
ID3D11DepthStencilView* GetDepthStencilView() const;
const D3D11_VIEWPORT& GetMainViewport() const;

// 렌더링 작업
void ClearRenderTargets(const float* ClearColor = nullptr);
void SwapBuffer();
void SetViewport(const D3D11_VIEWPORT& Viewport);

// 리소스 생성 헬퍼
bool CreateBuffer(const D3D11_BUFFER_DESC& Desc, const D3D11_SUBRESOURCE_DATA* InitialData, ID3D11Buffer** OutBuffer);
bool CreateVertexShader(const void* ByteCode, size_t CodeSize, ID3D11VertexShader** OutShader, ID3D11InputLayout** OutInputLayout, const D3D11_INPUT_ELEMENT_DESC* InputElements, uint32 NumElements);
bool CreatePixelShader(const void* ByteCode, size_t CodeSize, ID3D11PixelShader** OutShader);
```

#### 초기화 흐름 (D3D11GraphicsDevice.h:62-67)
```cpp
private:
    bool CreateDeviceAndContext();                        // 1. 디바이스/컨텍스트 생성
    bool CreateSwapChain(HWND, uint32, uint32, bool);     // 2. 스왑체인 생성
    bool CreateRenderTargetView();                        // 3. 백버퍼 RTV 생성
    bool CreateDepthStencilBuffer(uint32, uint32);        // 4. 깊이버퍼 생성
    bool CreateDepthStencilView();                        // 5. DSV 생성
    void SetupViewport(uint32, uint32);                   // 6. 뷰포트 설정
```

#### 디자인 패턴
- **Facade Pattern**: DirectX11 복잡한 API를 단순한 인터페이스로 래핑
- **RAII**: ComPtr을 사용한 자동 리소스 관리

---

## 2. Renderer Architecture

### 2.1 URenderer (Renderer.h:9)
**렌더링 오케스트레이터 - Command Pattern**

#### 역할
- 렌더 패스 관리 및 실행
- 씬 렌더링 조율
- 렌더 파이프라인 구성

#### 상속 관계
```
UObject
└── URenderer
```

#### 주요 멤버 변수
```cpp
ID3D11DeviceContext* DeviceContext;        // DirectX11 컨텍스트
FD3D11GraphicsDevice* GraphicsDevice;      // 그래픽 디바이스
TArray<IRenderPass*> RenderPasses;         // 렌더 패스 목록
bool bInitialized;                         // 초기화 플래그
```

#### 핵심 메서드
```cpp
// 초기화
void InitializeRenderer(ID3D11DeviceContext* InDeviceContext, FD3D11GraphicsDevice* InGraphicsDevice);
void Shutdown();

// 렌더링
void RenderScene();
void RenderSceneWithView(FSceneView* SceneView);

// 렌더 패스 관리
void AddRenderPass(IRenderPass* Pass);
void RemoveRenderPass(ERenderPassType PassType);
void ClearRenderPasses();

// 접근자
bool IsInitialized() const;
ID3D11DeviceContext* GetDeviceContext() const;
FD3D11GraphicsDevice* GetGraphicsDevice() const;

private:
    void SetupDefaultRenderPasses();
    void ExecuteRenderPass(IRenderPass* Pass, const FRenderPassContext& Context);
```

#### 디자인 패턴
- **Command Pattern**: RenderPass를 명령 객체로 관리
- **Strategy Pattern**: 다양한 렌더 패스 전략 교체 가능

---

### 2.2 IRenderPass (RenderPass.h:25)
**렌더 패스 인터페이스 - Strategy Pattern**

#### 역할
- 렌더 패스 추상화
- 다양한 렌더링 전략 정의

#### 핵심 메서드
```cpp
virtual void Execute(const FRenderPassContext& Context) = 0;
virtual ERenderPassType GetPassType() const = 0;
virtual const char* GetPassName() const = 0;
```

#### ERenderPassType (RenderPass.h:6)
```cpp
enum class ERenderPassType
{
    DepthPrePass,    // Z-PrePass (조기 깊이 테스트)
    BasePass         // 기본 렌더링 패스
};
```

#### 계층 구조
```
IRenderPass (Abstract Interface)
├── FDepthPrePass
└── FBasePass
```

---

### 2.3 FDepthPrePass (RenderPass.h:34)
**깊이 프리패스 - Early Z Rejection**

#### 역할
- 깊이 버퍼만 미리 렌더링
- 픽셀 셰이더 오버드로우 감소
- Early Z Rejection 최적화

#### 구현
```cpp
class FDepthPrePass : public IRenderPass
{
public:
    virtual void Execute(const FRenderPassContext& Context) override;
    virtual ERenderPassType GetPassType() const override { return ERenderPassType::DepthPrePass; }
    virtual const char* GetPassName() const override { return "DepthPrePass"; }
};
```

#### 렌더링 흐름
```
1. 깊이 스텐실 버퍼 바인딩 (RTV는 nullptr)
2. 깊이 전용 셰이더로 모든 지오메트리 렌더링
3. 깊이 버퍼에 Z값 기록
4. 결과: 씬의 깊이 정보 생성
```

---

### 2.4 FBasePass (RenderPass.h:42)
**기본 렌더링 패스 - Forward Rendering**

#### 역할
- 색상 + 라이팅 렌더링
- 머티리얼 셰이더 실행
- 최종 이미지 생성

#### 구현
```cpp
class FBasePass : public IRenderPass
{
public:
    virtual void Execute(const FRenderPassContext& Context) override;
    virtual ERenderPassType GetPassType() const override { return ERenderPassType::BasePass; }
    virtual const char* GetPassName() const override { return "BasePass"; }
};
```

#### 렌더링 흐름
```
1. RTV + DSV 바인딩
2. 깊이 테스트 활성화 (DepthPrePass 결과 사용)
3. 각 메시의 머티리얼 셰이더 실행
4. 색상 버퍼에 최종 색상 기록
```

---

### 2.5 FRenderPassContext (RenderPass.h:14)
**렌더 패스 실행 컨텍스트 - Context Object Pattern**

#### 역할
- 렌더 패스 실행에 필요한 모든 정보 전달
- 렌더 타겟, 뷰포트, 씬뷰 등 포함

#### 구조
```cpp
struct FRenderPassContext
{
    ERenderPassType PassType;                      // 패스 타입
    ID3D11DeviceContext* DeviceContext = nullptr;  // DirectX 컨텍스트
    ID3D11RenderTargetView* RenderTarget = nullptr; // 렌더 타겟
    ID3D11DepthStencilView* DepthStencil = nullptr; // 깊이 스텐실
    const D3D11_VIEWPORT* Viewport = nullptr;      // 뷰포트
    FSceneView* SceneView = nullptr;               // 씬 뷰 (카메라)
    float ClearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f }; // 클리어 색상
};
```

#### 디자인 패턴
- **Context Object Pattern**: 실행 컨텍스트를 단일 객체로 전달

---

## 3. Scene Management

### 3.1 FSceneView (SceneView.h:28)
**카메라 및 뷰포트 정보 - View Transform**

#### 역할
- 카메라 위치/회전 관리
- View/Projection 행렬 계산
- 뷰포트 설정 관리

#### 주요 멤버 변수
```cpp
// 카메라 트랜스폼
FVector ViewLocation;           // 카메라 위치
FVector ViewRotation;           // 카메라 회전 (Pitch, Yaw, Roll)

// 행렬 (캐싱)
FMatrix ViewMatrix;             // 뷰 행렬 (World → View)
FMatrix ProjectionMatrix;       // 투영 행렬 (View → Clip)
FMatrix ViewProjectionMatrix;   // VP 행렬 (World → Clip)

// 투영 설정
EProjectionType ProjectionType; // Perspective / Orthographic
float FOV;                      // 시야각 (Perspective)
float AspectRatio;              // 종횡비
float NearPlane;                // 근거리 클리핑
float FarPlane;                 // 원거리 클리핑
float OrthoWidth;               // 직교 투영 너비
float OrthoHeight;              // 직교 투영 높이

// 뷰포트
uint32 ViewportWidth;
uint32 ViewportHeight;

// Dirty Flags (지연 갱신)
bool bViewMatrixDirty;
bool bProjectionMatrixDirty;
```

#### EProjectionType (SceneView.h:7)
```cpp
enum class EProjectionType
{
    Perspective,    // 원근 투영
    Orthographic    // 직교 투영
};
```

#### 핵심 메서드
```cpp
// 생성자
FSceneView(const FSceneViewInitOptions& InitOptions);

// 카메라 설정
void SetViewLocation(const FVector& NewLocation);
void SetViewRotation(const FVector& NewRotation);
void SetProjectionType(EProjectionType NewType);
void SetFOV(float NewFOV);
void SetAspectRatio(float NewAspectRatio);
void SetNearFarPlanes(float NewNear, float NewFar);
void SetOrthoSize(float Width, float Height);
void SetViewportSize(uint32 Width, uint32 Height);

// 행렬 접근 (Lazy Evaluation)
const FMatrix& GetViewMatrix();
const FMatrix& GetProjectionMatrix();
const FMatrix& GetViewProjectionMatrix();

// 카메라 벡터
FVector GetViewDirection() const;   // Forward
FVector GetRightVector() const;     // Right
FVector GetUpVector() const;        // Up

// 행렬 갱신
void UpdateMatrices();

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
```

#### FSceneViewInitOptions (SceneView.h:13)
**씬뷰 초기화 옵션 - Builder Pattern**

```cpp
struct FSceneViewInitOptions
{
    FVector ViewLocation = FVector::Zero;
    FVector ViewRotation = FVector::Zero;
    EProjectionType ProjectionType = EProjectionType::Perspective;
    float FOV = 90.0f;
    float AspectRatio = 16.0f / 9.0f;
    float NearPlane = 1.0f;
    float FarPlane = 10000.0f;
    float OrthoWidth = 1024.0f;
    float OrthoHeight = 768.0f;
    uint32 ViewportWidth = 1920;
    uint32 ViewportHeight = 1080;
};
```

#### 좌표계 변환 흐름
```
Local Space (모델)
    ↓ (World Matrix)
World Space
    ↓ (View Matrix)
View Space (카메라)
    ↓ (Projection Matrix)
Clip Space
    ↓ (하드웨어 처리)
Screen Space
```

#### 디자인 패턴
- **Lazy Evaluation**: Dirty Flag로 필요할 때만 행렬 계산
- **Builder Pattern**: FSceneViewInitOptions로 초기화

---

## 4. Material System

### 4.1 UMaterialInterface (MaterialInterface.h:32)
**PBR 머티리얼 베이스 클래스**

#### 역할
- Physically Based Rendering (PBR) 머티리얼 정의
- 머티리얼 파라미터 관리
- 블렌드 모드, 셰이딩 모델 설정

#### 상속 관계
```
UMaterialInterface (Base Class)
└── UMaterialInstanceInterface (Dynamic Instance)
```

#### EBlendMode (MaterialInterface.h:3)
```cpp
enum class EBlendMode : uint8
{
    BLEND_Opaque,        // 불투명
    BLEND_Masked,        // 마스크 (알파 테스트)
    BLEND_Translucent,   // 반투명 (알파 블렌딩)
    BLEND_Additive,      // 가산 블렌딩
};
```

#### EShadingModel (MaterialInterface.h:11)
```cpp
enum class EShadingModel : uint8
{
    MSM_DefaultLit,      // 기본 라이팅
    MSM_Unlit,           // 라이팅 없음
};
```

#### 주요 멤버 변수
```cpp
// 렌더링 설정
EBlendMode BlendMode;
EShadingModel ShadingModel;
float Opacity;
float OpacityMaskClipValue;

// 머티리얼 플래그
bool bIsTwoSided;
bool bUsedWithStaticLighting;
bool bUsedWithInstancedStaticMeshes;

// PBR 파라미터
FVector BaseColor;          // 알베도 (기본 색상)
float Metallic;             // 금속성 (0=비금속, 1=금속)
float Specular;             // 반사율 (일반적으로 0.5)
float Roughness;            // 거칠기 (0=매끄러움, 1=거침)
FVector EmissiveColor;      // 발광 색상
FVector Normal;             // 법선 맵
FVector WorldPositionOffset; // WPO (정점 셰이더에서 사용)

// 텍스처 경로
FString DiffuseTexturePath;
FString NormalTexturePath;
FString SpecularTexturePath;
FString RoughnessTexturePath;
FString MetallicTexturePath;
FString EmissiveTexturePath;
FString OpacityTexturePath;

// 머티리얼 이름
FName MaterialName;
```

#### 핵심 메서드
```cpp
// 머티리얼 속성 조회
virtual bool IsUsedWithStaticLighting() const;
virtual bool IsUsedWithInstancedStaticMeshes() const;
virtual bool IsTwoSided() const;
virtual bool IsMasked() const;
virtual bool IsTranslucent() const;

// 파라미터 접근
virtual bool GetScalarParameterValue(const FMaterialParameterInfo& ParameterInfo, float& OutValue) const;
virtual bool GetVectorParameterValue(const FMaterialParameterInfo& ParameterInfo, FVector& OutValue) const;

// 리소스 관리
virtual void BeginDestroy();
virtual bool IsReadyForFinishDestroy();
virtual void FinishDestroy();

// 셰이더 컴파일
virtual bool IsCompilationFinished() const;
virtual void CacheShaders();

// 디버그
virtual FString GetDesc() const;

// 팩토리 메서드
static UMaterialInterface* CreateDefaultMaterial(const FName& Name);
static UMaterialInterface* CreatePBRMaterial(const FName& Name, const FVector& InBaseColor, float InMetallic, float InRoughness, float InSpecular);
```

#### FMaterialParameterInfo (MaterialInterface.h:17)
**머티리얼 파라미터 식별자**

```cpp
struct FMaterialParameterInfo
{
    FName Name;
    int32 Index;

    FMaterialParameterInfo(const FName& InName, int32 InIndex = 0);
};
```

#### 디자인 패턴
- **Factory Pattern**: CreateDefaultMaterial, CreatePBRMaterial
- **Template Method**: 가상 함수로 확장 가능한 구조

---

### 4.2 UMaterialInstanceInterface (MaterialInterface.h:189)
**동적 머티리얼 인스턴스 - Prototype Pattern**

#### 역할
- 부모 머티리얼 기반 파라미터 오버라이드
- 런타임 머티리얼 파라미터 변경
- 머티리얼 인스턴싱으로 메모리 절약

#### 상속 관계
```
UMaterialInterface
└── UMaterialInstanceInterface
```

#### 주요 멤버 변수
```cpp
UMaterialInterface* Parent;  // 부모 머티리얼

// 오버라이드된 파라미터
TArray<TPair<FName, float>> ScalarParameterOverrides;
TArray<TPair<FName, FVector>> VectorParameterOverrides;
```

#### 핵심 메서드
```cpp
// 파라미터 설정
void SetScalarParameterValue(const FName& ParameterName, float Value);
void SetVectorParameterValue(const FName& ParameterName, const FVector& Value);

// 파라미터 조회 (오버라이드 우선)
virtual bool GetScalarParameterValue(const FMaterialParameterInfo& ParameterInfo, float& OutValue) const override;
virtual bool GetVectorParameterValue(const FMaterialParameterInfo& ParameterInfo, FVector& OutValue) const override;
```

#### 파라미터 조회 우선순위
```
1. ScalarParameterOverrides / VectorParameterOverrides 검색
2. 없으면 Parent->GetXXXParameterValue() 호출
3. Parent도 없으면 UMaterialInterface 기본값 반환
```

#### 디자인 패턴
- **Prototype Pattern**: 부모 머티리얼 복제 및 수정
- **Decorator Pattern**: 부모 머티리얼에 파라미터 오버라이드 추가

---

## 5. Mesh System

### 5.1 UStaticMesh (StaticMesh.h:5)
**정적 메시 에셋 클래스**

#### 역할
- 메시 렌더 데이터 관리
- 머티리얼 슬롯 관리
- 렌더링 섹션 관리
- 바운딩 정보 제공

#### 상속 관계
```
UObject
└── UStaticMesh
```

#### 주요 멤버 변수
```cpp
FStaticMeshRenderData RenderData;       // 정점/인덱스 데이터
TArray<FStaticMaterial> StaticMaterials; // 머티리얼 슬롯
TArray<FStaticMeshSection> Sections;    // 렌더링 섹션
```

#### 핵심 메서드
```cpp
// 렌더 데이터
const FStaticMeshRenderData& GetRenderData() const;
void SetRenderData(const FStaticMeshRenderData& InRenderData);
void SetRenderData(const FString& InFilePath, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices);

// 머티리얼 슬롯
void AddMaterialSlot(const FName& SlotName, UMaterialInterface* Material);
void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material);
void SetMaterialByName(const FName& SlotName, UMaterialInterface* Material);
UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
UMaterialInterface* GetMaterialByName(const FName& SlotName) const;
int32 GetMaterialIndex(const FName& MaterialSlotName) const;

// 섹션 관리
void AddSection(uint32 MaterialIndex, uint32 FirstIndex, uint32 NumTriangles, uint32 MinVertexIndex, uint32 MaxVertexIndex);
void ClearSections();
const TArray<FStaticMeshSection>& GetSections() const;
int32 GetNumSections() const;

// 메시 정보
bool HasValidRenderData() const;

// 바운딩 정보
FVector GetBoundingBoxMin() const;
FVector GetBoundingBoxMax() const;
FVector GetBoundingBoxCenter() const;
FVector GetBoundingBoxExtent() const;
float GetBoundingSphereRadius() const;
const FBoxSphereBounds& GetBounds() const;

// 빌드
void BuildFromObjData(const FObjInfo& ObjData);
void BuildDefaultMaterialsAndSections();
```

---

### 5.2 FStaticMeshRenderData (StaticMeshRenderData.h:91)
**메시 렌더 데이터 - Data Transfer Object**

#### 역할
- 정점/인덱스 데이터 저장
- 바운딩 정보 계산
- GPU 업로드용 데이터 제공

#### 주요 멤버 변수
```cpp
FString SourceFilePath;         // 소스 파일 경로
TArray<FVertex> Vertices;       // 정점 배열
TArray<uint32> Indices;         // 인덱스 배열
uint32 NumVertices;             // 정점 개수
uint32 NumTriangles;            // 삼각형 개수
FBoxSphereBounds Bounds;        // 바운딩 정보
```

#### 핵심 메서드
```cpp
// 카운트 갱신
void UpdateCounts();

// 바운딩 갱신
void UpdateBounds();

// 바운딩 정보
FVector GetBoundingBoxMin() const;
FVector GetBoundingBoxMax() const;
FVector GetBoundingBoxCenter() const;
FVector GetBoundingBoxExtent() const;
float GetBoundingSphereRadius() const;
bool HasValidBounds() const;
```

---

### 5.3 FStaticMaterial (StaticMeshRenderData.h:49)
**머티리얼 슬롯 정보**

#### 역할
- 메시의 머티리얼 슬롯 정의
- 슬롯 이름과 머티리얼 매핑

#### 구조
```cpp
struct FStaticMaterial
{
    FName MaterialSlotName;           // 슬롯 이름
    UMaterialInterface* Material;     // 머티리얼 포인터
    FName ImportedMaterialSlotName;   // 원본 슬롯 이름
};
```

---

### 5.4 FStaticMeshSection (StaticMeshRenderData.h:66)
**렌더링 섹션 - Draw Call 단위**

#### 역할
- 머티리얼별 렌더링 구간 정의
- DrawIndexed 호출 파라미터 저장

#### 구조
```cpp
struct FStaticMeshSection
{
    uint32 MaterialIndex;     // 머티리얼 인덱스
    uint32 FirstIndex;        // 시작 인덱스
    uint32 NumTriangles;      // 삼각형 개수
    uint32 MinVertexIndex;    // 최소 정점 인덱스
    uint32 MaxVertexIndex;    // 최대 정점 인덱스
};
```

#### 렌더링 시 사용
```cpp
// DrawIndexed 호출
context->DrawIndexed(
    section.NumTriangles * 3,  // IndexCount
    section.FirstIndex,        // StartIndexLocation
    0                          // BaseVertexLocation
);
```

---

### 5.5 FVertex (Vertex.h:6)
**정점 구조체 - Vertex Format**

#### 역할
- 정점 데이터 정의
- 접선 공간(Tangent Space) 계산

#### 구조
```cpp
struct FVertex
{
    FVector Position;     // 위치 (Local Space)
    FVector Normal;       // 법선
    FVector Tangent;      // 접선
    FVector Binormal;     // 종법선 (Bitangent)
    FVector2 UV;          // 텍스처 좌표
};
```

#### 핵심 메서드
```cpp
// 생성자
FVertex();
FVertex(const FVector& InPosition, const FVector& InNormal, float InU, float InV);
FVertex(const FVector& InPosition, const FVector& InNormal, const FVector& InTangent, float InU, float InV);

private:
    void CalculateTangentSpace();  // 접선 공간 자동 계산
```

#### 접선 공간 계산 (Vertex.h:42)
```cpp
void CalculateTangentSpace()
{
    // 법선에 수직인 벡터 찾기
    FVector UpVector = FVector::Up;
    if (abs(Normal.Dot(UpVector)) > 0.9f)
        UpVector = FVector::Right;

    // Tangent = Up × Normal (외적)
    Tangent = UpVector.Cross(Normal).Normalize();

    // Binormal = Normal × Tangent (왼손 좌표계)
    Binormal = Normal.Cross(Tangent);
}
```

#### DirectX11 Input Layout 매핑
```cpp
D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

---

### 5.6 FObjInfo (StaticMeshRenderData.h:29)
**OBJ 파일 파싱 정보**

#### 역할
- OBJ 파일에서 파싱한 원시 데이터 저장
- 인덱스 기반 정점 데이터 관리

#### 구조
```cpp
struct FObjInfo
{
    FString ObjName;                         // 객체 이름
    TArray<FVector> VertexList;              // v (정점 위치)
    TArray<FVector2> UVList;                 // vt (UV 좌표)
    TArray<FVector> NormalList;              // vn (법선)
    TArray<uint32> VertexIndexList;          // f (정점 인덱스)
    TArray<uint32> UVIndexList;              // f (UV 인덱스)
    TArray<uint32> NormalIndexList;          // f (법선 인덱스)
    TArray<FObjMaterialInfo> MaterialList;   // mtl (머티리얼)
    TArray<FString> TextureList;             // 텍스처 경로
};
```

---

### 5.7 UKismetProceduralMeshLibrary (KismetProceduralMeshLibrary.h:9)
**프로시저럴 메시 생성기 - Factory Pattern**

#### 역할
- 기본 도형 메시 생성
- 런타임 메시 생성

#### 핵심 메서드
```cpp
// 기본 도형 생성
static FStaticMeshRenderData CreateCubeMesh(FVector BoxRadius = FVector(50, 50, 50));
static FStaticMeshRenderData CreateSphereMesh(float SphereRadius = 50.0f, int32 SphereSegments = 32, int32 SphereRings = 16);
static FStaticMeshRenderData CreateCylinderMesh(float CylinderRadius = 50.0f, float CylinderHeight = 100.0f, int32 CylinderSegments = 32);
static FStaticMeshRenderData CreateConeMesh(float ConeRadius = 50.0f, float ConeHeight = 100.0f, int32 ConeSegments = 32);
static FStaticMeshRenderData CreatePlaneMesh(FVector PlaneSize = FVector(100, 100, 0), int32 WidthSegments = 1, int32 HeightSegments = 1);
```

#### 디자인 패턴
- **Factory Pattern**: 정적 팩토리 메서드로 메시 생성
- **Utility Class**: 순수 정적 메서드만 제공

---

## Key Flows

### 렌더링 파이프라인 전체 흐름

```
1. 초기화 단계
   FD3D11GraphicsDevice::Init()
   ├─ CreateDeviceAndContext()
   ├─ CreateSwapChain()
   ├─ CreateRenderTargetView()
   ├─ CreateDepthStencilBuffer()
   ├─ CreateDepthStencilView()
   └─ SetupViewport()

   URenderer::InitializeRenderer()
   └─ SetupDefaultRenderPasses()
       ├─ AddRenderPass(new FDepthPrePass())
       └─ AddRenderPass(new FBasePass())

2. 렌더링 루프
   URenderer::RenderSceneWithView(SceneView)
   ├─ SceneView->UpdateMatrices()
   │   ├─ UpdateViewMatrix()
   │   └─ UpdateProjectionMatrix()
   │
   ├─ FRenderPassContext 생성
   │   ├─ DeviceContext 설정
   │   ├─ RenderTarget 설정
   │   ├─ DepthStencil 설정
   │   ├─ Viewport 설정
   │   └─ SceneView 설정
   │
   ├─ ExecuteRenderPass(DepthPrePass, Context)
   │   ├─ ClearDepthStencil()
   │   ├─ SetRenderTargets(nullptr, DSV)
   │   └─ 모든 메시 그리기 (깊이만)
   │
   └─ ExecuteRenderPass(BasePass, Context)
       ├─ SetRenderTargets(RTV, DSV)
       ├─ 각 메시별로:
       │   ├─ 머티리얼 셰이더 바인딩
       │   ├─ 상수 버퍼 업데이트 (MVP 행렬)
       │   └─ DrawIndexed()
       └─ SwapBuffer()
```

### 메시 렌더링 상세 흐름

```
1. 메시 로드/생성
   UStaticMesh::BuildFromObjData(ObjData)
   ├─ Vertices/Indices 생성
   ├─ FStaticMeshRenderData 구성
   ├─ Bounds 계산
   ├─ MaterialSlots 추가
   └─ Sections 생성

2. GPU 리소스 생성
   CreateVertexBuffer()
   ├─ D3D11_BUFFER_DESC 설정 (VERTEX_BUFFER)
   ├─ D3D11_SUBRESOURCE_DATA 설정 (Vertices)
   └─ Device->CreateBuffer()

   CreateIndexBuffer()
   ├─ D3D11_BUFFER_DESC 설정 (INDEX_BUFFER)
   ├─ D3D11_SUBRESOURCE_DATA 설정 (Indices)
   └─ Device->CreateBuffer()

3. 렌더링 (각 Section별)
   for (Section : Sections)
   {
       ├─ GetMaterial(Section.MaterialIndex)
       ├─ SetShader(Material->ShadingModel)
       ├─ UpdateConstantBuffer(ViewProjection, WorldMatrix, MaterialParams)
       ├─ IASetVertexBuffers()
       ├─ IASetIndexBuffer()
       └─ DrawIndexed(Section.NumTriangles * 3, Section.FirstIndex, 0)
   }
```

### 머티리얼 파라미터 조회 흐름

```
// UMaterialInstanceInterface::GetScalarParameterValue()

1. ScalarParameterOverrides 검색
   for (Override : ScalarParameterOverrides)
       if (Override.first == ParameterName)
           return Override.second;  // 인스턴스 오버라이드 반환

2. Parent->GetScalarParameterValue()
   if (Parent)
       return Parent->GetScalarParameterValue();  // 부모 머티리얼에서 조회

3. UMaterialInterface::GetScalarParameterValue()
   // 기본값 반환 (Metallic, Roughness 등)
   return DefaultValue;
```

### FSceneView 행렬 계산 흐름 (Lazy Evaluation)

```
1. GetViewMatrix() 호출
   if (bViewMatrixDirty)
       UpdateViewMatrix();
       ├─ Rotation → Forward/Right/Up 벡터 계산
       ├─ LookAt 행렬 생성
       │   ┌                           ┐
       │   │ Rx  Ry  Rz  -Dot(R, Pos) │
       │   │ Ux  Uy  Uz  -Dot(U, Pos) │
       │   │ Fx  Fy  Fz  -Dot(F, Pos) │
       │   │ 0   0   0    1           │
       │   └                           ┘
       └─ bViewMatrixDirty = false;
   return ViewMatrix;

2. GetProjectionMatrix() 호출
   if (bProjectionMatrixDirty)
       UpdateProjectionMatrix();
       if (ProjectionType == Perspective)
           ├─ PerspectiveFovLH 계산
           │   ┌                                    ┐
           │   │ w   0      0              0       │
           │   │ 0   h      0              0       │
           │   │ 0   0   f/(f-n)          1       │
           │   │ 0   0   -n*f/(f-n)       0       │
           │   └                                    ┘
       else if (ProjectionType == Orthographic)
           ├─ OrthographicLH 계산
       └─ bProjectionMatrixDirty = false;
   return ProjectionMatrix;

3. GetViewProjectionMatrix()
   ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
   return ViewProjectionMatrix;
```

---

## Design Patterns Summary

### 1. Facade Pattern
- **FD3D11GraphicsDevice**: DirectX11 복잡한 API를 간단한 인터페이스로 래핑

### 2. Command Pattern
- **IRenderPass**: 렌더 명령을 객체로 캡슐화
- **URenderer**: 렌더 패스들을 순차 실행

### 3. Strategy Pattern
- **IRenderPass**: 다양한 렌더링 전략 (DepthPrePass, BasePass)
- **EProjectionType**: 다양한 투영 방식 (Perspective, Orthographic)

### 4. Context Object Pattern
- **FRenderPassContext**: 렌더 패스 실행에 필요한 모든 정보를 단일 객체로 전달

### 5. Lazy Evaluation Pattern
- **FSceneView**: Dirty Flag로 필요할 때만 행렬 계산

### 6. Factory Pattern
- **UMaterialInterface::CreateDefaultMaterial()**: 기본 머티리얼 생성
- **UKismetProceduralMeshLibrary**: 프로시저럴 메시 생성

### 7. Prototype Pattern
- **UMaterialInstanceInterface**: 부모 머티리얼 기반 인스턴스 생성

### 8. Decorator Pattern
- **UMaterialInstanceInterface**: 부모 머티리얼에 파라미터 오버라이드 추가

### 9. Builder Pattern
- **FSceneViewInitOptions**: 복잡한 씬뷰 초기화를 단순화

### 10. Data Transfer Object (DTO) Pattern
- **FStaticMeshRenderData**: GPU 업로드용 데이터 전달
- **FRenderPassContext**: 렌더 패스 실행 데이터 전달

---

## Class Hierarchy Diagram

```
┌────────────────────────────────────────────────────────────┐
│                   Rendering Pipeline                        │
└────────────────────────────────────────────────────────────┘
                          │
        ┌─────────────────┼──────────────────┬─────────────┐
        │                 │                  │             │
        ▼                 ▼                  ▼             ▼
  ┌──────────┐      ┌──────────┐      ┌──────────┐  ┌──────────┐
  │   RHI    │      │ Renderer │      │  Scene   │  │ Material │
  └──────────┘      └──────────┘      └──────────┘  └──────────┘
        │                 │                  │             │
        ▼                 ▼                  ▼             ▼
FD3D11Graphics    URenderer          FSceneView    UMaterial
    Device         │                                Interface
                   │                                      │
             ┌─────┴─────┐                               │
             ▼           ▼                               ▼
        IRenderPass  FRenderPass                UMaterial
             │        Context                   Instance
        ┌────┴────┐
        ▼         ▼
  FDepthPre  FBasePass
    Pass
```

### 상세 클래스 계층

```
RHI Layer
└── FD3D11GraphicsDevice
    ├── ID3D11Device (DirectX)
    ├── ID3D11DeviceContext (DirectX)
    └── IDXGISwapChain (DirectX)

Renderer
└── UObject
    └── URenderer
        └── TArray<IRenderPass*>
            ├── FDepthPrePass
            └── FBasePass

Scene
├── FSceneView
│   ├── FMatrix (View/Projection)
│   └── EProjectionType
└── FRenderPassContext

Material
└── UMaterialInterface
    ├── EBlendMode
    ├── EShadingModel
    └── UMaterialInstanceInterface

Mesh
└── UObject
    └── UStaticMesh
        ├── FStaticMeshRenderData
        │   ├── TArray<FVertex>
        │   ├── TArray<uint32> (Indices)
        │   └── FBoxSphereBounds
        ├── TArray<FStaticMaterial>
        └── TArray<FStaticMeshSection>
```

---

## 다른 시스템과의 관계

### Rendering Pipeline → Core Systems
- URenderer는 UObject 상속
- UStaticMesh는 UObject 상속
- FName을 머티리얼/슬롯 이름으로 사용
- GUObjectArray에서 렌더 대상 검색

### Rendering Pipeline → Engine Core
- UPrimitiveComponent에서 UStaticMesh 참조
- AActor가 컴포넌트 렌더링 요청
- UWorld가 씬 렌더링 조율

### Rendering Pipeline → Editor
- FEditorViewportClient가 FSceneView 생성
- SEditorViewport가 렌더 타겟 제공
- 에디터가 머티리얼 편집 인터페이스 제공

---

## 참고: 클래스 매핑

| 분류 | 클래스/구조체 | 주요 역할 |
|-----|-------------|---------|
| **RHI** | FD3D11GraphicsDevice | DirectX11 래퍼 |
| **Renderer** | URenderer | 렌더링 오케스트레이터 |
| **Renderer** | IRenderPass | 렌더 패스 인터페이스 |
| **Renderer** | FDepthPrePass | 깊이 프리패스 |
| **Renderer** | FBasePass | 기본 렌더링 패스 |
| **Renderer** | FRenderPassContext | 렌더 패스 컨텍스트 |
| **Scene** | FSceneView | 카메라/뷰포트 |
| **Scene** | FSceneViewInitOptions | 씬뷰 초기화 옵션 |
| **Material** | UMaterialInterface | PBR 머티리얼 |
| **Material** | UMaterialInstanceInterface | 동적 머티리얼 인스턴스 |
| **Mesh** | UStaticMesh | 정적 메시 에셋 |
| **Mesh** | FStaticMeshRenderData | 메시 렌더 데이터 |
| **Mesh** | FVertex | 정점 구조 |
| **Mesh** | FStaticMaterial | 머티리얼 슬롯 |
| **Mesh** | FStaticMeshSection | 렌더링 섹션 |
| **Mesh** | UKismetProceduralMeshLibrary | 프로시저럴 메시 생성 |

---

## 학습 체크리스트

- [ ] DirectX11 초기화 흐름 이해 (Device, SwapChain, RTV, DSV)
- [ ] 렌더 패스 시스템 이해 (DepthPrePass → BasePass)
- [ ] FSceneView의 행렬 계산 이해 (View, Projection, VP)
- [ ] PBR 머티리얼 파라미터 이해 (BaseColor, Metallic, Roughness)
- [ ] 머티리얼 인스턴싱 메커니즘 이해
- [ ] 메시 렌더 데이터 구조 이해 (Vertices, Indices, Sections)
- [ ] 접선 공간(Tangent Space) 계산 이해
- [ ] 렌더링 파이프라인 전체 흐름 숙지
- [ ] Z-Up Left-handed 좌표계 이해

---

**다음 단계**: Phase 3 - Engine Core 분석
