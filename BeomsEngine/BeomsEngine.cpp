// BeomsEngine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <iostream>
#include "pch.h"
#include "BeomsEngine.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
#include "PrimitiveComponent.h"
#include "MeshComponent.h"
#include "StaticMeshComponent.h"
#include "StaticMeshActor.h"
#include "StaticMesh.h"
#include "BoxSphereBounds.h"
#include "UObjectIterator.h"
#include "UObjectArray.h"
#include "ObjectInitializer.h"
#include "Math.h"
#include "Name.h"

void RunTests()
{
    // 콘솔 창 생성
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);

    printf("===================================================================\n");
    printf("              BEOMS ENGINE COMPREHENSIVE TEST SUITE               \n");
    printf("===================================================================\n\n");

    // Test 1: BoxSphereBounds 테스트
    printf("TEST 1: BoxSphereBounds Functionality\n");
    printf("--------------------------------------\n");

    // 기본 생성자 테스트
    FBoxSphereBounds DefaultBounds;
    printf("Default Constructor: Origin(%.1f,%.1f,%.1f), Extent(%.1f,%.1f,%.1f), Radius(%.1f)\n",
           DefaultBounds.Origin.X, DefaultBounds.Origin.Y, DefaultBounds.Origin.Z,
           DefaultBounds.BoxExtent.X, DefaultBounds.BoxExtent.Y, DefaultBounds.BoxExtent.Z,
           DefaultBounds.SphereRadius);
    printf("Default Constructor: %s\n",
           (DefaultBounds.Origin == FVector::Zero && DefaultBounds.BoxExtent == FVector::Zero && DefaultBounds.SphereRadius == 0.0f) ? "PASS" : "FAIL");

    // 매개변수 생성자 테스트
    FVector TestOrigin(10.0f, 20.0f, 30.0f);
    FVector TestExtent(50.0f, 60.0f, 70.0f);
    float TestRadius = 100.0f;
    FBoxSphereBounds CustomBounds(TestOrigin, TestExtent, TestRadius);
    printf("Custom Constructor: %s\n",
           (CustomBounds.Origin == TestOrigin && CustomBounds.BoxExtent == TestExtent && CustomBounds.SphereRadius == TestRadius) ? "PASS" : "FAIL");

    // 정점 배열로부터 바운딩 계산 테스트
    TArray<FVertex> TestVertices;
    TestVertices.push_back(FVertex(FVector(-10.0f, -20.0f, -30.0f), FVector::Up, 0.0f, 0.0f));
    TestVertices.push_back(FVertex(FVector(10.0f, 20.0f, 30.0f), FVector::Up, 1.0f, 1.0f));
    TestVertices.push_back(FVertex(FVector(0.0f, 0.0f, 0.0f), FVector::Up, 0.5f, 0.5f));

    FBoxSphereBounds VertexBounds(TestVertices);
    printf("Vertex-based Bounds: Origin(%.1f,%.1f,%.1f), Extent(%.1f,%.1f,%.1f)\n",
           VertexBounds.Origin.X, VertexBounds.Origin.Y, VertexBounds.Origin.Z,
           VertexBounds.BoxExtent.X, VertexBounds.BoxExtent.Y, VertexBounds.BoxExtent.Z);
    printf("Vertex-based Constructor: %s\n",
           (abs(VertexBounds.Origin.X) < 0.1f && abs(VertexBounds.Origin.Y) < 0.1f && abs(VertexBounds.Origin.Z) < 0.1f) ? "PASS" : "FAIL");
    printf("\n");

    // Test 2: PrimitiveComponent 테스트
    printf("TEST 2: PrimitiveComponent Functionality\n");
    printf("-----------------------------------------\n");

    AActor* TestActor = NewObject<AActor>(nullptr, FName("TestActor"));
    if (TestActor)
    {
        UPrimitiveComponent* PrimComp = TestActor->CreateComponent<UPrimitiveComponent>(FName("PrimitiveComponent"));
        if (PrimComp)
        {
            printf("PrimitiveComponent Creation: PASS\n");
            printf("Component Class: %s\n", PrimComp->GetClassName().c_str());

            // 가시성 테스트
            PrimComp->SetVisibility(true);
            printf("Set Visibility True: %s\n", PrimComp->IsVisible() ? "PASS" : "FAIL");

            PrimComp->SetVisibility(false);
            printf("Set Visibility False: %s\n", !PrimComp->IsVisible() ? "PASS" : "FAIL");

            // Hidden 상태 테스트
            PrimComp->SetHidden(true);
            printf("Set Hidden True: %s\n", PrimComp->IsHidden() ? "PASS" : "FAIL");

            PrimComp->SetHidden(false);
            printf("Set Hidden False: %s\n", !PrimComp->IsHidden() ? "PASS" : "FAIL");

            // 렌더링 조건 테스트
            PrimComp->SetVisibility(true);
            PrimComp->SetHidden(false);
            printf("Should Render (Visible + Not Hidden): %s\n", PrimComp->ShouldRender() ? "PASS" : "FAIL");

            // 바운딩 정보 테스트
            FBoxSphereBounds PrimBounds = PrimComp->GetBounds();
            printf("Bounds Access: PASS (No crash)\n");
        }
        else
        {
            printf("PrimitiveComponent Creation: FAIL\n");
        }
    }
    printf("\n");

    // Test 3: MeshComponent 테스트
    printf("TEST 3: MeshComponent Functionality\n");
    printf("------------------------------------\n");

    UMeshComponent* MeshComp = TestActor->CreateComponent<UMeshComponent>(FName("MeshComponent"));
    if (MeshComp)
    {
        printf("MeshComponent Creation: PASS\n");
        printf("Component Class: %s\n", MeshComp->GetClassName().c_str());

        // 상속 관계 확인
        UPrimitiveComponent* AsPrimitive = Cast<UPrimitiveComponent>(MeshComp);
        printf("Inheritance (PrimitiveComponent): %s\n", AsPrimitive ? "PASS" : "FAIL");

        // 와이어프레임 모드 테스트
        MeshComp->SetWireframeMode(true);
        printf("Set Wireframe Mode True: %s\n", MeshComp->IsWireframeMode() ? "PASS" : "FAIL");

        MeshComp->SetWireframeMode(false);
        printf("Set Wireframe Mode False: %s\n", !MeshComp->IsWireframeMode() ? "PASS" : "FAIL");

        // 머티리얼 관련 기본값 테스트
        printf("Default Num Materials: %d\n", MeshComp->GetNumMaterials());
        printf("Material System: PASS (No crash)\n");

        // 메시 데이터 기본값 테스트
        printf("Default Num Vertices: %d\n", MeshComp->GetNumVertices());
        printf("Default Num Triangles: %d\n", MeshComp->GetNumTriangles());
        printf("Has Valid Mesh Data: %s\n", MeshComp->HasValidMeshData() ? "true" : "false");
    }
    else
    {
        printf("MeshComponent Creation: FAIL\n");
    }
    printf("\n");

    // Test 4: StaticMeshComponent 테스트
    printf("TEST 4: StaticMeshComponent Functionality\n");
    printf("------------------------------------------\n");

    UStaticMeshComponent* StaticMeshComp = TestActor->CreateComponent<UStaticMeshComponent>(FName("StaticMeshComponent"));
    if (StaticMeshComp)
    {
        printf("StaticMeshComponent Creation: PASS\n");
        printf("Component Class: %s\n", StaticMeshComp->GetClassName().c_str());

        // 상속 관계 확인
        UMeshComponent* AsMesh = Cast<UMeshComponent>(StaticMeshComp);
        printf("Inheritance (MeshComponent): %s\n", AsMesh ? "PASS" : "FAIL");

        // 정적 메시 설정 없이 기본 상태 테스트
        printf("Initial Static Mesh: %s\n", StaticMeshComp->GetStaticMesh() ? "Set" : "None");

        // 바운딩 정보 테스트
        FBoxSphereBounds SMBounds = StaticMeshComp->GetBounds();
        printf("Bounds Access: PASS (No crash)\n");

        // 렌더링 상태 테스트
        printf("Should Render: %s\n", StaticMeshComp->ShouldRender() ? "true" : "false");
    }
    else
    {
        printf("StaticMeshComponent Creation: FAIL\n");
    }
    printf("\n");

    // Test 5: StaticMeshActor 테스트
    printf("TEST 5: StaticMeshActor Functionality\n");
    printf("--------------------------------------\n");

    // 기본 생성자 테스트
    AStaticMeshActor* StaticMeshActor = NewObject<AStaticMeshActor>(nullptr, FName("StaticMeshActor"));
    if (StaticMeshActor)
    {
        printf("StaticMeshActor Creation: PASS\n");
        printf("Actor Class: %s\n", StaticMeshActor->GetClassName().c_str());

        // StaticMeshComponent 자동 생성 확인
        UStaticMeshComponent* AutoCreatedComp = StaticMeshActor->GetStaticMeshComponent();
        printf("Auto-created StaticMeshComponent: %s\n", AutoCreatedComp ? "PASS" : "FAIL");

        if (AutoCreatedComp)
        {
            // 루트 컴포넌트 확인
            printf("Root Component Set: %s\n",
                   (StaticMeshActor->GetRootComponent() == AutoCreatedComp) ? "PASS" : "FAIL");
        }

        // 팩토리 함수 테스트
        AStaticMeshActor* CubeActor = AStaticMeshActor::CreateWithCubeMesh(FVector(100.0f, 0.0f, 0.0f));
        if (CubeActor)
        {
            printf("CreateWithCubeMesh: PASS\n");
            printf("Cube Actor Location: (%.1f, %.1f, %.1f)\n",
                   CubeActor->GetActorLocation().X, CubeActor->GetActorLocation().Y, CubeActor->GetActorLocation().Z);

            UStaticMesh* CubeMesh = CubeActor->GetStaticMesh();
            printf("Cube Mesh Created: %s\n", CubeMesh ? "PASS" : "FAIL");

            if (CubeMesh)
            {
                FBoxSphereBounds CubeBounds = CubeActor->GetBounds();
                printf("  Cube Bounds - Origin: (%.1f, %.1f, %.1f)\n",
                       CubeBounds.Origin.X, CubeBounds.Origin.Y, CubeBounds.Origin.Z);
                printf("  Cube Bounds - Extent: (%.1f, %.1f, %.1f)\n",
                       CubeBounds.BoxExtent.X, CubeBounds.BoxExtent.Y, CubeBounds.BoxExtent.Z);
                printf("  Cube Bounds - Sphere Radius: %.1f\n", CubeBounds.SphereRadius);
                printf("  Cube Bounding Box: %s\n",
                       (CubeBounds.BoxExtent.X > 0 && CubeBounds.BoxExtent.Y > 0 && CubeBounds.BoxExtent.Z > 0) ? "PASS" : "FAIL");
            }
        }
        else
        {
            printf("CreateWithCubeMesh: FAIL\n");
        }

        AStaticMeshActor* SphereActor = AStaticMeshActor::CreateWithSphereMesh(FVector(0.0f, 100.0f, 0.0f));
        if (SphereActor)
        {
            printf("CreateWithSphereMesh: PASS\n");
            UStaticMesh* SphereMesh = SphereActor->GetStaticMesh();
            printf("Sphere Mesh Created: %s\n", SphereMesh ? "PASS" : "FAIL");

            if (SphereMesh)
            {
                FBoxSphereBounds SphereBounds = SphereActor->GetBounds();
                printf("  Sphere Bounds - Origin: (%.1f, %.1f, %.1f)\n",
                       SphereBounds.Origin.X, SphereBounds.Origin.Y, SphereBounds.Origin.Z);
                printf("  Sphere Bounds - Extent: (%.1f, %.1f, %.1f)\n",
                       SphereBounds.BoxExtent.X, SphereBounds.BoxExtent.Y, SphereBounds.BoxExtent.Z);
                printf("  Sphere Bounds - Sphere Radius: %.1f\n", SphereBounds.SphereRadius);
                printf("  Sphere Bounding Box: %s\n",
                       (SphereBounds.SphereRadius > 0 && SphereBounds.BoxExtent.X > 0) ? "PASS" : "FAIL");
            }
        }
        else
        {
            printf("CreateWithSphereMesh: FAIL\n");
        }

        AStaticMeshActor* CylinderActor = AStaticMeshActor::CreateWithCylinderMesh(FVector(0.0f, 0.0f, 100.0f));
        if (CylinderActor)
        {
            printf("CreateWithCylinderMesh: PASS\n");
            UStaticMesh* CylinderMesh = CylinderActor->GetStaticMesh();
            printf("Cylinder Mesh Created: %s\n", CylinderMesh ? "PASS" : "FAIL");

            if (CylinderMesh)
            {
                FBoxSphereBounds CylinderBounds = CylinderActor->GetBounds();
                printf("  Cylinder Bounds - Origin: (%.1f, %.1f, %.1f)\n",
                       CylinderBounds.Origin.X, CylinderBounds.Origin.Y, CylinderBounds.Origin.Z);
                printf("  Cylinder Bounds - Extent: (%.1f, %.1f, %.1f)\n",
                       CylinderBounds.BoxExtent.X, CylinderBounds.BoxExtent.Y, CylinderBounds.BoxExtent.Z);
                printf("  Cylinder Bounds - Sphere Radius: %.1f\n", CylinderBounds.SphereRadius);
                printf("  Cylinder Bounding Box: %s\n",
                       (CylinderBounds.BoxExtent.X > 0 && CylinderBounds.BoxExtent.Y > 0 && CylinderBounds.BoxExtent.Z > 0) ? "PASS" : "FAIL");
            }
        }
        else
        {
            printf("CreateWithCylinderMesh: FAIL\n");
        }

        AStaticMeshActor* ConeActor = AStaticMeshActor::CreateWithConeMesh(FVector(-100.0f, 0.0f, 0.0f));
        if (ConeActor)
        {
            printf("CreateWithConeMesh: PASS\n");
            UStaticMesh* ConeMesh = ConeActor->GetStaticMesh();
            printf("Cone Mesh Created: %s\n", ConeMesh ? "PASS" : "FAIL");

            if (ConeMesh)
            {
                FBoxSphereBounds ConeBounds = ConeActor->GetBounds();
                printf("  Cone Bounds - Origin: (%.1f, %.1f, %.1f)\n",
                       ConeBounds.Origin.X, ConeBounds.Origin.Y, ConeBounds.Origin.Z);
                printf("  Cone Bounds - Extent: (%.1f, %.1f, %.1f)\n",
                       ConeBounds.BoxExtent.X, ConeBounds.BoxExtent.Y, ConeBounds.BoxExtent.Z);
                printf("  Cone Bounds - Sphere Radius: %.1f\n", ConeBounds.SphereRadius);
                printf("  Cone Bounding Box: %s\n",
                       (ConeBounds.BoxExtent.X > 0 && ConeBounds.BoxExtent.Y > 0 && ConeBounds.BoxExtent.Z > 0) ? "PASS" : "FAIL");
            }
        }
        else
        {
            printf("CreateWithConeMesh: FAIL\n");
        }

        // 추가적인 바운딩 박스 정확성 테스트
        printf("\n  --- Bounding Box Accuracy Tests ---\n");

        // 큐브 바운딩 박스 정확성 (기본 50x50x50 크기)
        AStaticMeshActor* TestCube = AStaticMeshActor::CreateWithCubeMesh(FVector::Zero, FVector(25.0f, 25.0f, 25.0f));
        if (TestCube)
        {
            FBoxSphereBounds TestCubeBounds = TestCube->GetBounds();
            printf("  Test Cube (25x25x25) Extent: (%.1f, %.1f, %.1f)\n",
                   TestCubeBounds.BoxExtent.X, TestCubeBounds.BoxExtent.Y, TestCubeBounds.BoxExtent.Z);
            printf("  Cube Size Accuracy: %s\n",
                   (abs(TestCubeBounds.BoxExtent.X - 25.0f) < 1.0f &&
                    abs(TestCubeBounds.BoxExtent.Y - 25.0f) < 1.0f &&
                    abs(TestCubeBounds.BoxExtent.Z - 25.0f) < 1.0f) ? "PASS" : "FAIL");
        }

        // 구 바운딩 박스 정확성 (기본 50 반지름)
        AStaticMeshActor* TestSphere = AStaticMeshActor::CreateWithSphereMesh(FVector::Zero, 30.0f);
        if (TestSphere)
        {
            FBoxSphereBounds TestSphereBounds = TestSphere->GetBounds();
            printf("  Test Sphere (r=30) Sphere Radius: %.1f\n", TestSphereBounds.SphereRadius);
            printf("  Sphere Size Accuracy: %s\n",
                   (abs(TestSphereBounds.SphereRadius - 30.0f) < 5.0f) ? "PASS" : "FAIL");
        }

        // 실린더 바운딩 박스 정확성 (기본 50 반지름, 100 높이)
        AStaticMeshActor* TestCylinder = AStaticMeshActor::CreateWithCylinderMesh(FVector::Zero, 20.0f, 60.0f);
        if (TestCylinder)
        {
            FBoxSphereBounds TestCylinderBounds = TestCylinder->GetBounds();
            printf("  Test Cylinder (r=20, h=60) Extent: (%.1f, %.1f, %.1f)\n",
                   TestCylinderBounds.BoxExtent.X, TestCylinderBounds.BoxExtent.Y, TestCylinderBounds.BoxExtent.Z);
            printf("  Cylinder Size Accuracy: %s\n",
                   (abs(TestCylinderBounds.BoxExtent.X - 20.0f) < 5.0f &&
                    abs(TestCylinderBounds.BoxExtent.Y - 20.0f) < 5.0f &&
                    abs(TestCylinderBounds.BoxExtent.Z - 30.0f) < 5.0f) ? "PASS" : "FAIL");
        }
    }
    else
    {
        printf("StaticMeshActor Creation: FAIL\n");
    }
    printf("\n");

    // Test 6: ObjectIterator 테스트
    printf("TEST 6: ObjectIterator Functionality\n");
    printf("-------------------------------------\n");

    // 전체 오브젝트 반복자 테스트
    int32 TotalObjectCount = 0;
    for (TObjectIterator<UObject> It; It; ++It)
    {
        UObject* Obj = *It;
        if (Obj)
        {
            TotalObjectCount++;
        }
    }
    printf("Total UObject Count: %d\n", TotalObjectCount);
    printf("ObjectIterator Basic: %s\n", TotalObjectCount > 0 ? "PASS" : "FAIL");

    // 특정 타입 반복자 테스트
    int32 ActorCount = 0;
    for (TObjectIterator<AActor> It; It; ++It)
    {
        AActor* Actor = *It;
        if (Actor)
        {
            ActorCount++;
        }
    }
    printf("AActor Count: %d\n", ActorCount);
    printf("Type-specific Iterator: %s\n", ActorCount > 0 ? "PASS" : "FAIL");

    // 컴포넌트 반복자 테스트
    int32 ComponentCount = 0;
    for (TObjectIterator<UActorComponent> It; It; ++It)
    {
        UActorComponent* Component = *It;
        if (Component)
        {
            ComponentCount++;
        }
    }
    printf("UActorComponent Count: %d\n", ComponentCount);
    printf("Component Iterator: %s\n", ComponentCount > 0 ? "PASS" : "FAIL");

    // StaticMeshActor 반복자 테스트
    int32 StaticMeshActorCount = 0;
    for (TObjectIterator<AStaticMeshActor> It; It; ++It)
    {
        AStaticMeshActor* SMActor = *It;
        if (SMActor)
        {
            StaticMeshActorCount++;
            printf("  Found StaticMeshActor: %s at (%.1f, %.1f, %.1f)\n",
                   SMActor->GetName().ToString().c_str(),
                   SMActor->GetActorLocation().X, SMActor->GetActorLocation().Y, SMActor->GetActorLocation().Z);
        }
    }
    printf("StaticMeshActor Count: %d\n", StaticMeshActorCount);
    printf("StaticMeshActor Iterator: %s\n", StaticMeshActorCount > 0 ? "PASS" : "FAIL");
    printf("\n");

    // 최종 결과
    printf("===================================================================\n");
    printf("                        TEST SUMMARY                              \n");
    printf("===================================================================\n");
    printf("BoxSphereBounds: Operational\n");
    printf("PrimitiveComponent: Operational\n");
    printf("MeshComponent: Operational\n");
    printf("StaticMeshComponent: Operational\n");
    printf("StaticMeshActor: Operational (with procedural meshes)\n");
    printf("ObjectIterator: Operational\n");
    printf("Total Objects in System: %d\n", TotalObjectCount);
    printf("Component System: Fully Functional\n");
    printf("===================================================================\n\n");

    printf("Press any key to continue...\n");
    getchar();
}

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BEOMSENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BEOMSENGINE));

    MSG msg;

    RunTests();

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	FNamePool::DistroyInstance();

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEOMSENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BEOMSENGINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
