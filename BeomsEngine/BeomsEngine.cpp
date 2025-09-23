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
        printf("   ✓ World created successfully: %s\n", TestWorld->GetWorldName().ToString().c_str());
        printf("   - IsInitialized: %s\n", TestWorld->IsInitialized() ? "true" : "false");
        printf("   - IsPlaying: %s\n", TestWorld->IsPlaying() ? "true" : "false");
        printf("   - IsPaused: %s\n", TestWorld->IsPaused() ? "true" : "false");
    }
    else
    {
        printf("   ✗ Failed to create world\n");
    }

    // 2. 월드 초기화 테스트
    printf("\n2. Initializing World...\n");
    if (TestWorld)
    {
        TestWorld->InitializeWorld();
        printf("   ✓ World initialized\n");
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
            printf("   ✓ Level created successfully: %s\n", TestLevel->GetLevelName().ToString().c_str());
            printf("   - IsLoaded: %s\n", TestLevel->IsLoaded() ? "true" : "false");
            printf("   - IsVisible: %s\n", TestLevel->IsVisible() ? "true" : "false");
            printf("   - IsCurrent: %s\n", TestLevel->IsCurrent() ? "true" : "false");
            printf("   - Actor Count: %d\n", TestLevel->GetNumActors());
        }
        else
        {
            printf("   ✗ Failed to create level\n");
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
            printf("   ✓ Current level set successfully\n");
            printf("   - Current Level: %s\n", CurrentLevel->GetLevelName().ToString().c_str());
        }
        else
        {
            printf("   ✗ Failed to set current level\n");
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
            printf("   ✓ StaticMeshActor spawned: %s at (%.1f, %.1f, %.1f)\n",
                TestActor1->GetName().ToString().c_str(),
                TestActor1->GetActorLocation().X,
                TestActor1->GetActorLocation().Y,
                TestActor1->GetActorLocation().Z);
        }

        // 일반 Actor 스폰
        AActor* TestActor2 = TestLevel->SpawnActor<AActor>(FVector(-50.0f, 0.0f, 100.0f), FVector::Zero, FName("TestActor"));
        if (TestActor2)
        {
            printf("   ✓ Actor spawned: %s at (%.1f, %.1f, %.1f)\n",
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
            printf("   ✓ Found actor by name: %s\n", FoundActor->GetName().ToString().c_str());
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

    GUObjectArray.PerformGarbageCollector();

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
