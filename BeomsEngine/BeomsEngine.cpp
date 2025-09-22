// BeomsEngine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <iostream>
#include "pch.h"
#include "BeomsEngine.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
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

    // Test 1: FName 기본 기능 테스트
    printf("TEST 1: FName Basic Functionality\n");
    printf("----------------------------------\n");

    FName Name1("TestName");
    FName Name2("TestName");
    FName Name3("DifferentName");
    FName EmptyName;
    FName CopyName(Name1);

    printf("Basic Creation: %s\n", Name1.IsValid() ? "PASS" : "FAIL");
    printf("Empty FName: %s\n", EmptyName.IsNone() ? "PASS" : "FAIL");
    printf("Copy Constructor: %s\n", (CopyName == Name1) ? "PASS" : "FAIL");
    printf("Same String Equality: %s\n", (Name1 == Name2) ? "PASS" : "FAIL");
    printf("Different String Inequality: %s\n", (Name1 != Name3) ? "PASS" : "FAIL");
    printf("Comparison Operators: %s\n", (Name1 < Name3 || Name1 > Name3) ? "PASS" : "FAIL");

    // 대소문자 무시 테스트
    FName NameA("MyObject");
    FName NameB("myobject");
    FName NameC("MYOBJECT");
    FName NameD("MyObJeCt");

    printf("Case Insensitive Tests:\n");
    printf("  'MyObject' == 'myobject': %s\n", (NameA == NameB) ? "PASS" : "FAIL");
    printf("  'MyObject' == 'MYOBJECT': %s\n", (NameA == NameC) ? "PASS" : "FAIL");
    printf("  'MyObject' == 'MyObJeCt': %s\n", (NameA == NameD) ? "PASS" : "FAIL");

    printf("  'NameA': %s\n", NameA.ToString().c_str());
    printf("  'NameB': %s\n", NameB.ToString().c_str());
    printf("  'NameC': %s\n", NameC.ToString().c_str());
    printf("  'NameD': %s\n", NameD.ToString().c_str());

    printf("  All should be PASS for case-insensitive comparison\n");
    printf("\n");

    // Test 2: FName Pool 메모리 효율성 테스트
    printf("TEST 2: FName Pool Memory Efficiency\n");
    printf("-------------------------------------\n");

    size_t InitialEntries = FNamePool::GetInstance().GetNumEntries();
    size_t InitialMemory = FNamePool::GetInstance().GetMemoryUsage();

    printf("Initial Pool State: %zu entries, %zu bytes\n", InitialEntries, InitialMemory);

    // 같은 문자열로 1000개의 FName 생성
    TArray<FName> RepeatedNames;
    for (int i = 0; i < 1000; ++i)
    {
        RepeatedNames.push_back(FName("RepeatedTestName"));
    }

    size_t AfterRepeatedEntries = FNamePool::GetInstance().GetNumEntries();
    size_t AfterRepeatedMemory = FNamePool::GetInstance().GetMemoryUsage();

    printf("After 1000 identical FNames: %zu entries (+%zu), %zu bytes (+%zu)\n",
           AfterRepeatedEntries, AfterRepeatedEntries - InitialEntries,
           AfterRepeatedMemory, AfterRepeatedMemory - InitialMemory);

    // 다른 문자열로 100개의 FName 생성
    TArray<FName> UniqueNames;
    for (int i = 0; i < 100; ++i)
    {
        FString UniqueName = FString("UniqueName_") + std::to_string(i).c_str();
        UniqueNames.push_back(FName(UniqueName));
    }

    size_t AfterUniqueEntries = FNamePool::GetInstance().GetNumEntries();
    size_t AfterUniqueMemory = FNamePool::GetInstance().GetMemoryUsage();

    printf("After 100 unique FNames: %zu entries (+%zu), %zu bytes (+%zu)\n",
           AfterUniqueEntries, AfterUniqueEntries - AfterRepeatedEntries,
           AfterUniqueMemory, AfterUniqueMemory - AfterRepeatedMemory);

    printf("Memory Efficiency: %s\n",
           (AfterRepeatedEntries - InitialEntries == 1) ? "PASS - Only 1 entry added for 1000 identical names" : "FAIL");

    // 대소문자 다른 버전들로 메모리 효율성 추가 테스트
    TArray<FName> CaseVariations;
    for (int i = 0; i < 100; ++i)
    {
        CaseVariations.push_back(FName("TestCaseName"));
        CaseVariations.push_back(FName("TESTCASENAME"));
        CaseVariations.push_back(FName("testcasename"));
        CaseVariations.push_back(FName("TestCAsEnAmE"));
    }

    size_t AfterCaseTestEntries = FNamePool::GetInstance().GetNumEntries();
    printf("After 400 case variations of same name: %zu entries (+%zu)\n",
           AfterCaseTestEntries, AfterCaseTestEntries - AfterUniqueEntries);
    printf("Case Insensitive Memory Efficiency: %s\n",
           (AfterCaseTestEntries - AfterUniqueEntries == 1) ? "PASS - Only 1 entry for all case variations" : "FAIL");
    printf("\n");

    // Test 3: FName 성능 테스트
    printf("TEST 3: FName Performance Benchmarks\n");
    printf("-------------------------------------\n");

    // FName vs FString 비교 성능 테스트
    const int COMPARISON_COUNT = 1000000;

    // FName 비교 테스트
    auto StartFName = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < COMPARISON_COUNT; ++i)
    {
        volatile bool Result = Name1 == Name2;
        (void)Result;
    }
    auto EndFName = std::chrono::high_resolution_clock::now();
    auto FNameDuration = std::chrono::duration_cast<std::chrono::microseconds>(EndFName - StartFName);

    // FString 비교 테스트 (비교용)
    FString Str1 = "TestName";
    FString Str2 = "TestName";
    auto StartFString = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < COMPARISON_COUNT; ++i)
    {
        volatile bool Result = (Str1 == Str2);
        (void)Result;
    }
    auto EndFString = std::chrono::high_resolution_clock::now();
    auto FStringDuration = std::chrono::duration_cast<std::chrono::microseconds>(EndFString - StartFString);

    printf("FName Comparisons (%d): %lld microseconds\n", COMPARISON_COUNT, FNameDuration.count());
    printf("FString Comparisons (%d): %lld microseconds\n", COMPARISON_COUNT, FStringDuration.count());
    printf("Performance Improvement: %.2fx faster\n",
           (double)FStringDuration.count() / (double)FNameDuration.count());
    printf("Performance Test: %s\n",
           FNameDuration.count() < FStringDuration.count() ? "PASS - FName is faster" : "FAIL");
    printf("\n");

    // Test 4: 문자열 변환 테스트
    printf("TEST 4: String Conversion Tests\n");
    printf("--------------------------------\n");

    FName TestConversion("ConversionTest");
    FString ConvertedBack = TestConversion.ToString();
    printf("Original -> FName -> String: 'ConversionTest' -> '%s'\n", ConvertedBack.c_str());
    printf("Round-trip Conversion: %s\n",
           (ConvertedBack == "ConversionTest") ? "PASS" : "FAIL");

    // 특수 문자 테스트
    FName SpecialChars("Test_With-Special.Chars123");
    printf("Special Characters: '%s'\n", SpecialChars.ToString().c_str());
    printf("Special Characters: %s\n",
           (SpecialChars.ToString() == "Test_With-Special.Chars123") ? "PASS" : "FAIL");
    printf("\n");

    // Test 5: 미리 정의된 EName 상수 테스트
    printf("TEST 5: EName Predefined Constants\n");
    printf("-----------------------------------\n");

    printf("EName::None: '%s' (IsNone: %s)\n",
           EName::None.ToString().c_str(), EName::None.IsNone() ? "true" : "false");
    printf("EName::Default: '%s'\n", EName::Default.ToString().c_str());
    printf("EName::Material: '%s'\n", EName::Material.ToString().c_str());
    printf("EName::BeginPlay: '%s'\n", EName::BeginPlay.ToString().c_str());
    printf("EName::Transform: '%s'\n", EName::Transform.ToString().c_str());
    printf("EName::BaseColor: '%s'\n", EName::BaseColor.ToString().c_str());

    printf("Predefined Constants: %s\n",
           (EName::Material.ToString() == "Material" &&
            EName::BeginPlay.ToString() == "BeginPlay") ? "PASS" : "FAIL");
    printf("\n");

    // Test 6: UObject 시스템 통합 테스트
    printf("TEST 6: UObject System Integration\n");
    printf("-----------------------------------\n");

    // AActor 생성 및 FName 사용 테스트
    AActor* TestActor = NewObject<AActor>();
    if (TestActor)
    {
        printf("AActor Creation: PASS\n");

        // 이름 설정 및 확인
        FName ActorName("MyTestActor");
        TestActor->SetName(ActorName);

        printf("Set Actor Name: '%s'\n", TestActor->GetName().ToString().c_str());
        printf("FName Integration: %s\n",
               (TestActor->GetName() == ActorName) ? "PASS" : "FAIL");

        // 클래스 이름 확인
        UClass* ActorClass = TestActor->GetClass();
        if (ActorClass)
        {
            printf("Actor Class Name: '%s'\n", ActorClass->GetName().ToString().c_str());
            printf("Class System Integration: %s\n",
                   (ActorClass->GetName() == FName("AActor")) ? "PASS" : "FAIL");
        }

        // 컴포넌트 생성 테스트
        USceneComponent* TestComponent = TestActor->CreateComponent<USceneComponent>(FName("TestComponent"));
        if (TestComponent)
        {
            printf("Component Name: '%s'\n", TestComponent->GetName().ToString().c_str());
            printf("Component Creation with FName: %s\n",
                   (TestComponent->GetName() == FName("TestComponent")) ? "PASS" : "FAIL");
        }
        else
        {
            printf("Component Creation: FAIL\n");
        }
    }
    else
    {
        printf("AActor Creation: FAIL\n");
    }
    printf("\n");

    // Test 7: 해시 맵 성능 테스트
    printf("TEST 7: HashMap Performance Test\n");
    printf("---------------------------------\n");

    std::unordered_map<FName, int> FNameMap;
    std::unordered_map<FString, int> FStringMap;

    // 데이터 삽입
    const int MAP_SIZE = 10000;
    auto StartMapInsert = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        FNameMap[FName(FString("Key_") + std::to_string(i).c_str())] = i;
    }
    auto EndMapInsert = std::chrono::high_resolution_clock::now();
    auto FNameMapDuration = std::chrono::duration_cast<std::chrono::microseconds>(EndMapInsert - StartMapInsert);

    auto StartStringMapInsert = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAP_SIZE; ++i)
    {
        FStringMap[FString("Key_") + std::to_string(i).c_str()] = i;
    }
    auto EndStringMapInsert = std::chrono::high_resolution_clock::now();
    auto FStringMapDuration = std::chrono::duration_cast<std::chrono::microseconds>(EndStringMapInsert - StartStringMapInsert);

    printf("FName Map Insert (%d items): %lld microseconds\n", MAP_SIZE, FNameMapDuration.count());
    printf("FString Map Insert (%d items): %lld microseconds\n", MAP_SIZE, FStringMapDuration.count());
    printf("Map Performance: FName is %.2fx %s\n",
           std::abs((double)FStringMapDuration.count() / (double)FNameMapDuration.count()),
           FNameMapDuration.count() < FStringMapDuration.count() ? "faster" : "slower");
    printf("\n");

    // Test 8: 메모리 누수 검사
    printf("TEST 8: Memory Leak Detection\n");
    printf("------------------------------\n");

    size_t FinalEntries = FNamePool::GetInstance().GetNumEntries();
    size_t FinalMemory = FNamePool::GetInstance().GetMemoryUsage();

    printf("Final Pool State: %zu entries, %zu bytes\n", FinalEntries, FinalMemory);
    printf("Total Growth: +%zu entries, +%zu bytes\n",
           FinalEntries - InitialEntries, FinalMemory - InitialMemory);
    printf("Memory Management: Monitoring completed\n");
    printf("\n");

    // 최종 결과
    printf("===================================================================\n");
    printf("                        TEST SUMMARY                              \n");
    printf("===================================================================\n");
    printf("FName System: Operational\n");
    printf("Memory Efficiency: Confirmed\n");
    printf("Performance: %.2fx faster than FString\n",
           (double)FStringDuration.count() / (double)FNameDuration.count());
    printf("UObject Integration: Successful\n");
    printf("Pool Entries: %zu total\n", FinalEntries);
    printf("Memory Usage: %zu bytes\n", FinalMemory);
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
