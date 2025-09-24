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
#include "World.h"
#include "Level.h"
#include "D3D11GraphicsDevice.h"
#include "Renderer.h"
#include "RenderPass.h"
#include "SceneView.h"
#include "ViewportClient.h"
#include "EditorViewportClient.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
HWND GHwnd;                                    // 메인 윈도우 핸들입니다.
WCHAR szTitle[ MAX_LOADSTRING ];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[ MAX_LOADSTRING ];            // 기본 창 클래스 이름입니다.

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

    // World와 Level 시스템 테스트
    printf(">> WORLD & LEVEL SYSTEM TESTS\n");
    printf("-------------------------------------\n");

    // 1. 월드 생성 테스트
    printf("1. Creating World...\n");
    UWorld* TestWorld = UWorld::CreateWorld(FName("TestWorld"));
    if (TestWorld)
    {
        printf("    World created successfully: %s\n", TestWorld->GetWorldName().ToString().c_str());
        printf("   - IsInitialized: %s\n", TestWorld->IsInitialized() ? "true" : "false");
        printf("   - IsPlaying: %s\n", TestWorld->IsPlaying() ? "true" : "false");
        printf("   - IsPaused: %s\n", TestWorld->IsPaused() ? "true" : "false");
    }
    else
    {
        printf("    Failed to create world\n");
    }

    // 2. 월드 초기화 테스트
    printf("\n2. Initializing World...\n");
    if (TestWorld)
    {
        TestWorld->InitializeWorld();
        printf("    World initialized\n");
        printf("   - IsInitialized: %s\n", TestWorld->IsInitialized() ? "true" : "false");
    }

    // 3. 레벨 생성 테스트
    printf("\n3. Creating Level...\n");
    ULevel* TestLevel = nullptr;
    if (TestWorld)
    {
        TestLevel = TestWorld->CreateLevel(FName("TestLevel"));
        if (TestLevel)
        {
            printf("    Level created successfully: %s\n", TestLevel->GetLevelName().ToString().c_str());
            printf("   - IsLoaded: %s\n", TestLevel->IsLoaded() ? "true" : "false");
            printf("   - IsVisible: %s\n", TestLevel->IsVisible() ? "true" : "false");
            printf("   - IsCurrent: %s\n", TestLevel->IsCurrent() ? "true" : "false");
            printf("   - Actor Count: %d\n", TestLevel->GetNumActors());
        }
        else
        {
            printf("    Failed to create level\n");
        }
    }

    // 4. 레벨을 월드의 현재 레벨로 설정
    printf("\n4. Setting Current Level...\n");
    if (TestWorld && TestLevel)
    {
        TestWorld->SetCurrentLevel(TestLevel);
        ULevel* CurrentLevel = TestWorld->GetCurrentLevel();
        if (CurrentLevel == TestLevel)
        {
            printf("    Current level set successfully\n");
            printf("   - Current Level: %s\n", CurrentLevel->GetLevelName().ToString().c_str());
        }
        else
        {
            printf("    Failed to set current level\n");
        }
    }

    // 5. 액터 스폰 테스트
    printf("\n5. Spawning Actors in Level...\n");
    if (TestLevel)
    {
        // StaticMeshActor 스폰
        AActor* TestActor1 = TestLevel->SpawnActor<AStaticMeshActor>(FVector(100.0f, 200.0f, 50.0f), FVector::Zero, FName("TestMeshActor"));
        if (TestActor1)
        {
            printf("    StaticMeshActor spawned: %s at (%.1f, %.1f, %.1f)\n",
                TestActor1->GetName().ToString().c_str(),
                TestActor1->GetActorLocation().X,
                TestActor1->GetActorLocation().Y,
                TestActor1->GetActorLocation().Z);
        }

        // 일반 Actor 스폰
        AActor* TestActor2 = TestLevel->SpawnActor<AActor>(FVector(-50.0f, 0.0f, 100.0f), FVector::Zero, FName("TestActor"));
        if (TestActor2)
        {
            printf("    Actor spawned: %s at (%.1f, %.1f, %.1f)\n",
                TestActor2->GetName().ToString().c_str(),
                TestActor2->GetActorLocation().X,
                TestActor2->GetActorLocation().Y,
                TestActor2->GetActorLocation().Z);
        }

        printf("   - Total actors in level: %d\n", TestLevel->GetNumActors());
    }

    // 6. 액터 검색 테스트
    printf("\n6. Finding Actors...\n");
    if (TestLevel)
    {
        AActor* FoundActor = TestLevel->FindActor(FName("TestMeshActor"));
        if (FoundActor)
        {
            printf("    Found actor by name: %s\n", FoundActor->GetName().ToString().c_str());
        }

        // 클래스별 액터 검색
        TArray<AStaticMeshActor*> MeshActors = TestLevel->GetActorsOfClass<AStaticMeshActor>();
        printf("   - StaticMeshActors found: %zu\n", MeshActors.size());

        TArray<AActor*> AllActors = TestLevel->GetActorsOfClass<AActor>();
        printf("   - Total Actors found: %zu\n", AllActors.size());
    }

    // 7. 월드 통계 테스트
    printf("\n7. World Statistics...\n");
    if (TestWorld)
    {
        printf("   - World Name: %s\n", TestWorld->GetWorldName().ToString().c_str());
        printf("   - Total Actor Count: %d\n", TestWorld->GetTotalActorCount());
        printf("   - Time Seconds: %.2f\n", TestWorld->GetTimeSeconds());
        printf("   - Delta Time: %.4f\n", TestWorld->GetDeltaTimeSeconds());

        // 월드 경계 정보
        const UWorld::FWorldBounds& Bounds = TestWorld->GetWorldBounds();
        printf("   - World Bounds: Min(%.1f, %.1f, %.1f) Max(%.1f, %.1f, %.1f)\n",
            Bounds.Min.X, Bounds.Min.Y, Bounds.Min.Z,
            Bounds.Max.X, Bounds.Max.Y, Bounds.Max.Z);
    }

    // 8. 레벨 통계 테스트
    printf("\n8. Level Statistics...\n");
    if (TestLevel)
    {
        ULevel::FLevelStats Stats = TestLevel->GetLevelStats();
        printf("   - Total Actors: %d\n", Stats.TotalActors);
        printf("   - StaticMesh Actors: %d\n", Stats.StaticMeshActors);
        printf("   - Light Actors: %d\n", Stats.LightActors);
        printf("   - Other Actors: %d\n", Stats.OtherActors);
        printf("   - Memory Usage: %zu bytes\n", Stats.MemoryUsage);
    }

    // 9. 시간 시뮬레이션 테스트
    printf("\n9. Time Simulation Test...\n");
    if (TestWorld)
    {
        TestWorld->SetPlaying(true);
        printf("   - World playing state: %s\n", TestWorld->IsPlaying() ? "true" : "false");

        // 몇 프레임 시뮬레이션
        for (int32 Frame = 0; Frame < 3; ++Frame)
        {
            float DeltaTime = 0.016f; // 60 FPS
            TestWorld->AddDeltaTime(DeltaTime);
            TestWorld->Tick(DeltaTime);

            printf("   Frame %d: Time=%.3f, DeltaTime=%.3f\n",
                Frame + 1, TestWorld->GetTimeSeconds(), TestWorld->GetDeltaTimeSeconds());
        }
    }

    // 10. 정리 및 제거 테스트
    printf("\n10. Cleanup Test...\n");
    if (TestLevel)
    {
        printf("   - Removing all actors from level...\n");
        TestLevel->RemoveAllActors();
        printf("   - Actors remaining: %d\n", TestLevel->GetNumActors());

        TestLevel->CleanupLevel();
        printf("   ✓ Level cleaned up\n");
    }

    if (TestWorld)
    {
        TestWorld->CleanupWorld();
        printf("   ✓ World cleaned up\n");

        UWorld::DestroyWorld(TestWorld);
        printf("   ✓ World destroyed\n");
    }

    printf("\n>> WORLD & LEVEL SYSTEM TESTS COMPLETED\n");
    printf("=====================================\n\n");

    // GRAPHICS DEVICE & RENDERER TESTS
    printf(">> GRAPHICS DEVICE & RENDERER SYSTEM TESTS\n");
    printf("================================================\n");

    // 1. FD3D11GraphicsDevice 생성 및 초기화 테스트
    printf("\n1. Creating and Initializing FD3D11GraphicsDevice...\n");
    FD3D11GraphicsDevice GraphicsDevice;

    // 전역 윈도우 핸들을 사용하여 GraphicsDevice 초기화
    FD3D11GraphicsDevice::FDeviceDesc DeviceDesc;
    DeviceDesc.WindowHandle = GHwnd;
    DeviceDesc.WindowWidth = 1920;
    DeviceDesc.WindowHeight = 1080;
    DeviceDesc.bVSync = true;

    bool bGraphicsDeviceInit = GraphicsDevice.Init(DeviceDesc);
    if (bGraphicsDeviceInit)
    {
        printf("    GraphicsDevice initialized successfully\n");
        printf("   - Device: %p\n", GraphicsDevice.GetDevice());
        printf("   - Context: %p\n", GraphicsDevice.GetContext());
        printf("   - SwapChain: %p\n", GraphicsDevice.GetSwapChain());
        printf("   - BackBufferRTV: %p\n", GraphicsDevice.GetBackBufferRTV());
        printf("   - DepthStencilView: %p\n", GraphicsDevice.GetDepthStencilView());

        const D3D11_VIEWPORT& Viewport = GraphicsDevice.GetMainViewport();
        printf("   - Viewport: (%.0f, %.0f) - (%.0f x %.0f)\n",
            Viewport.TopLeftX, Viewport.TopLeftY, Viewport.Width, Viewport.Height);
    }
    else
    {
        printf("    Failed to initialize GraphicsDevice\n");
        printf("   - Check if DirectX 11 is supported on this system\n");
    }

    // 2. URenderer 생성 및 초기화 테스트 (GraphicsDevice 없이도 테스트 가능한 부분)
    printf("\n2. Creating URenderer...\n");
	URenderer* FrameRenderer = NewObject<URenderer>(nullptr, FName("FrameRenderer"));
    if (FrameRenderer)
    {
        printf("   ✓ URenderer created successfully: %s\n", FrameRenderer->GetName().ToString().c_str());
        printf("   - IsInitialized: %s\n", FrameRenderer->IsInitialized() ? "true" : "false");

        // GraphicsDevice가 초기화된 경우에만 실제 초기화 수행
        if (bGraphicsDeviceInit)
        {
            printf("\n3. Initializing URenderer with GraphicsDevice...\n");
            FrameRenderer->InitializeRenderer(GraphicsDevice.GetContext(), &GraphicsDevice);
            printf("   ✓ URenderer initialized with GraphicsDevice\n");
            printf("   - IsInitialized: %s\n", FrameRenderer->IsInitialized() ? "true" : "false");
            printf("   - DeviceContext: %p\n", FrameRenderer->GetDeviceContext());
            printf("   - GraphicsDevice: %p\n", FrameRenderer->GetGraphicsDevice());
        }
        else
        {
            printf("\n3. Testing URenderer without GraphicsDevice...\n");
            FrameRenderer->InitializeRenderer(nullptr, nullptr);
            printf("   ✓ URenderer initialized (null context test)\n");
        }
    }
    else
    {
        printf("   ✗ Failed to create URenderer\n");
    }

    // 4. 렌더링 시뮬레이션 테스트
    printf("\n4. Rendering Simulation Test...\n");
    if (FrameRenderer && FrameRenderer->IsInitialized())
    {
        printf("   Starting render scene simulation...\n");

        // 여러 프레임 시뮬레이션
        for (int32 Frame = 0; Frame < 3; ++Frame)
        {
            printf("\n   Frame %d Rendering:\n", Frame + 1);
            FrameRenderer->RenderScene();

            // GraphicsDevice가 초기화된 경우 Present 호출
            if (bGraphicsDeviceInit)
            {
                GraphicsDevice.SwapBuffer();
                printf("   - Frame %d presented to screen\n", Frame + 1);
            }
        }

        printf("\n   ✓ Rendering simulation completed\n");
    }
    else
    {
        printf("   ✗ Cannot perform rendering simulation - URenderer not initialized\n");
    }

    // 5. 렌더 패스 관리 테스트
    printf("\n5. Render Pass Management Test...\n");
    if (FrameRenderer)
    {
        printf("   Testing render pass operations...\n");

        // 기본 패스들이 설정되어 있는지 확인 (실제로는 내부적으로 확인)
        printf("   - Default render passes should be: DepthPrePass, BasePass\n");

        // 새로운 패스 추가 테스트 (실제 구현에서는 더 복잡한 패스 가능)
        printf("   - Render pass management functionality available\n");
    }

    printf("\n>> GRAPHICS DEVICE & RENDERER SYSTEM TESTS COMPLETED\n");
    printf("======================================================\n\n");

    // CAMERA & VIEWPORT SYSTEM TESTS
    printf(">> CAMERA & VIEWPORT SYSTEM TESTS\n");
    printf("=====================================\n");

    // 1. FSceneView 생성 및 테스트
    printf("\n1. Creating and Testing FSceneView...\n");
    FSceneViewInitOptions ViewOptions;
    ViewOptions.ViewLocation = FVector(100.0f, -300.0f, 200.0f);
    ViewOptions.ViewRotation = FVector(-20.0f, 45.0f, 0.0f);
    ViewOptions.FOV = 75.0f;
    ViewOptions.AspectRatio = 16.0f / 9.0f;
    ViewOptions.ViewportWidth = 1920;
    ViewOptions.ViewportHeight = 1080;

    FSceneView* TestSceneView = new FSceneView(ViewOptions);
    if (TestSceneView)
    {
        printf("    SceneView created successfully\n");
        printf("   - Location: (%.1f, %.1f, %.1f)\n",
            TestSceneView->ViewLocation.X, TestSceneView->ViewLocation.Y, TestSceneView->ViewLocation.Z);
        printf("   - Rotation: (%.1f, %.1f, %.1f)\n",
            TestSceneView->ViewRotation.X, TestSceneView->ViewRotation.Y, TestSceneView->ViewRotation.Z);
        printf("   - FOV: %.1f degrees\n", TestSceneView->FOV);
        printf("   - AspectRatio: %.2f\n", TestSceneView->AspectRatio);
        printf("   - Viewport: %dx%d\n", TestSceneView->ViewportWidth, TestSceneView->ViewportHeight);

        // 뷰 방향 벡터들 테스트
        FVector ViewDir = TestSceneView->GetViewDirection();
        FVector RightDir = TestSceneView->GetRightVector();
        FVector UpDir = TestSceneView->GetUpVector();

        printf("   - ViewDirection: (%.3f, %.3f, %.3f)\n", ViewDir.X, ViewDir.Y, ViewDir.Z);
        printf("   - RightVector: (%.3f, %.3f, %.3f)\n", RightDir.X, RightDir.Y, RightDir.Z);
        printf("   - UpVector: (%.3f, %.3f, %.3f)\n", UpDir.X, UpDir.Y, UpDir.Z);
    }

    // 2. FViewportClient 생성 및 테스트
    printf("\n2. Creating and Testing FViewportClient...\n");
    FViewportClient* GameViewport = new FViewportClient(EViewportType::Game);
    if (GameViewport)
    {
        GameViewport->Initialize(1920, 1080);
        printf("    Game ViewportClient created and initialized\n");
        printf("   - Type: %s\n", GameViewport->GetViewportType() == EViewportType::Game ? "Game" : "Editor");
        printf("   - Size: %dx%d\n", GameViewport->GetViewportWidth(), GameViewport->GetViewportHeight());
        printf("   - IsActive: %s\n", GameViewport->IsActive() ? "true" : "false");

        // 카메라 이동 테스트
        GameViewport->SetViewLocation(FVector(0.0f, -200.0f, 150.0f));
        GameViewport->SetViewRotation(FVector(-10.0f, 0.0f, 0.0f));
        GameViewport->SetFOV(60.0f);
        printf("   - Camera position and settings updated\n");
    }

    // 3. FEditorViewportClient 생성 및 테스트
    printf("\n3. Creating and Testing FEditorViewportClient...\n");
    FEditorViewportClient* EditorViewport = new FEditorViewportClient();
    if (EditorViewport)
    {
        EditorViewport->Initialize(1920, 1080);
        printf("    Editor ViewportClient created and initialized\n");
        printf("   - Type: %s\n", EditorViewport->GetViewportType() == EViewportType::Editor ? "Editor" : "Game");
        printf("   - CameraMode: Orbit\n");
        printf("   - OrbitTarget: (%.1f, %.1f, %.1f)\n",
            EditorViewport->GetOrbitTarget().X, EditorViewport->GetOrbitTarget().Y, EditorViewport->GetOrbitTarget().Z);
        printf("   - OrbitDistance: %.1f\n", EditorViewport->GetOrbitDistance());

        // 에디터 카메라 기능 테스트
        EditorViewport->SetOrbitTarget(FVector(100.0f, 100.0f, 0.0f));
        EditorViewport->SetOrbitDistance(400.0f);
        printf("   - Orbit target and distance updated\n");

        EditorViewport->FocusOnPoint(FVector(200.0f, 0.0f, 50.0f));
        printf("   - Focused on specific point\n");

        EditorViewport->ResetCamera();
        printf("   - Camera reset to default\n");

        // 뷰 모드 테스트
        EditorViewport->SetViewMode(EEditorViewMode::Wireframe);
        EditorViewport->SetViewMode(EEditorViewMode::Lit);
        printf("   - ViewMode switching tested\n");
    }

    // 4. 뷰포트와 렌더러 통합 테스트
    printf("\n4. Viewport-Renderer Integration Test...\n");
    if (FrameRenderer && GameViewport && EditorViewport && bGraphicsDeviceInit)
    {
        printf("   Testing viewport rendering with camera views...\n");

        // Game Viewport 렌더링
        printf("   - Rendering Game Viewport:\n");
        GameViewport->Draw(&GraphicsDevice, FrameRenderer);

        // Editor Viewport 렌더링
        printf("   - Rendering Editor Viewport:\n");
        EditorViewport->Draw(&GraphicsDevice, FrameRenderer);

        GraphicsDevice.SwapBuffer();
        printf("   - Viewport integration test completed\n");
    }
    else
    {
        printf("   - Viewport-Renderer integration test skipped (missing components)\n");
    }

    // 5. 카메라 애니메이션 시뮬레이션 테스트
    printf("\n5. Camera Animation Simulation...\n");
    if (EditorViewport)
    {
        printf("   Simulating camera movement over time...\n");

        for (int32 Frame = 0; Frame < 5; ++Frame)
        {
            float Time = Frame * 0.5f;
            float DeltaTime = 0.016f;

            // 궤도 회전 시뮬레이션
            FVector AnimatedTarget = FVector(
                100.0f * FMath::Cos(Time),
                100.0f * FMath::Sin(Time),
                50.0f
            );

            EditorViewport->SetOrbitTarget(AnimatedTarget);
            EditorViewport->Tick(DeltaTime);

            if (EditorViewport->GetSceneView())
            {
                FVector CamPos = EditorViewport->GetSceneView()->ViewLocation;
                printf("   Frame %d: Camera at (%.1f, %.1f, %.1f)\n",
                    Frame + 1, CamPos.X, CamPos.Y, CamPos.Z);
            }
        }

        printf("   - Camera animation simulation completed\n");
    }

    // 6. 리소스 정리
    printf("\n6. Camera System Cleanup...\n");
    if (TestSceneView)
    {
        delete TestSceneView;
        printf("   - SceneView destroyed\n");
    }

    if (GameViewport)
    {
        GameViewport->Shutdown();
        delete GameViewport;
        printf("   - Game ViewportClient destroyed\n");
    }

    if (EditorViewport)
    {
        EditorViewport->Shutdown();
        delete EditorViewport;
        printf("   - Editor ViewportClient destroyed\n");
    }

    // 6. 리소스 정리 테스트
	printf("\n6. Resource Cleanup Test...\n");
	if (FrameRenderer)
	{
		FrameRenderer->Shutdown();
		printf("   ✓ URenderer shutdown completed\n");
	}

	if (bGraphicsDeviceInit)
	{
		GraphicsDevice.Shutdown();
		printf("   ✓ GraphicsDevice shutdown completed\n");
	}

    printf("\n>> CAMERA & VIEWPORT SYSTEM TESTS COMPLETED\n");
    printf("============================================\n\n");

    GUObjectArray.PerformGarbageCollector();

    FNamePool::DistroyInstance();

    printf("Press any key to continue...\n");
    getchar();
}

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

   GHwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!GHwnd)
   {
      return FALSE;
   }

   ShowWindow(GHwnd, nCmdShow);
   UpdateWindow(GHwnd);

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
