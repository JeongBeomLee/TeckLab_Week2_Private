// BeomsEngine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "BeomsEngine.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
#include "UObjectArray.h"
#include "ObjectInitializer.h"
#include "Math.h"
#include <iostream>

// 테스트용 커스텀 액터 클래스
class ATestActor : public AActor
{
public:
    ATestActor() : AActor()
    {
        SetName(TEXT("TestActor"));
    }

    explicit ATestActor(const FObjectInitializer& ObjectInitializer)
        : AActor(ObjectInitializer)
    {
        if (GetName().empty())
        {
            SetName(TEXT("TestActor"));
        }
    }

    virtual FString GetClassName() const override { return TEXT("ATestActor"); }

    static UClass* GetStaticClass()
    {
        static UClass* StaticClass = nullptr;
        if (!StaticClass)
        {
            StaticClass = new UClass(TEXT("ATestActor"), AActor::GetStaticClass(), &ATestActor::CreateInstance);
            UClass::RegisterClass(StaticClass);
        }
        return StaticClass;
    }

    virtual UClass* GetClass() const override
    {
        return ATestActor::GetStaticClass();
    }

    static UObject* CreateInstance()
    {
        return new ATestActor();
    }
};

// 테스트 함수들
void TestBasicObjectInitializer()
{
    std::cout << "\n=== Testing Basic FObjectInitializer ===\n";

    // Create basic FObjectInitializer
    FObjectInitializer ObjectInit;
    ObjectInit.SetName(TEXT("TestObject"))
        .SetOuter(nullptr)
        .SetClass(UObject::GetStaticClass());

    // Create object
    UObject* TestObj = ObjectInit.CreateObject();
    if (TestObj)
    {
        std::cout << "FObjectInitializer::CreateObject() Success\n";
        std::cout << "  Object Name: " << TestObj->GetName().c_str() << "\n";
        std::cout << "  UniqueID: " << TestObj->GetUniqueID() << "\n";
        std::cout << "  InternalIndex: " << TestObj->GetInternalIndex() << "\n";

        delete TestObj;
        std::cout << "Object deletion completed\n";
    }
    else
    {
        std::cout << "FObjectInitializer::CreateObject() Failed\n";
    }
}

void TestNewObjectFunction()
{
    std::cout << "\n=== Testing NewObject Function ===\n";

    // NewObject<T> test
    UObject* TestObj1 = NewObject<UObject>(nullptr, TEXT("NewObjectTest1"));
    if (TestObj1)
    {
        std::cout << "NewObject<UObject>() Success\n";
        std::cout << "  Object Name: " << TestObj1->GetName().c_str() << "\n";
        std::cout << "  Class Name: " << TestObj1->GetClassName().c_str() << "\n";

        delete TestObj1;
    }

    // NewObject<AActor> test
    AActor* TestActor = NewObject<AActor>(nullptr, TEXT("NewActorTest"));
    if (TestActor)
    {
        std::cout << "NewObject<AActor>() Success\n";
        std::cout << "  Object Name: " << TestActor->GetName().c_str() << "\n";
        std::cout << "  Class Name: " << TestActor->GetClassName().c_str() << "\n";
        std::cout << "  RootComponent: " << (TestActor->GetRootComponent() ? "Exists" : "None") << "\n";

        delete TestActor;
    }

    // NewObject<UActorComponent> test
    UActorComponent* TestComponent = NewObject<UActorComponent>(nullptr, TEXT("NewComponentTest"));
    if (TestComponent)
    {
        std::cout << "NewObject<UActorComponent>() Success\n";
        std::cout << "  Object Name: " << TestComponent->GetName().c_str() << "\n";
        std::cout << "  Class Name: " << TestComponent->GetClassName().c_str() << "\n";

        if (TestComponent->IsActive())
        {
            std::cout << "  Active State: Active\n";
        }
        else
        {
            std::cout << "  Active State: Inactive\n";
        }

        delete TestComponent;
    }
}

void TestActorWithComponents()
{
    std::cout << "\n=== Testing Actor with Components Creation ===\n";

    // Create actor with FObjectInitializer
    FObjectInitializer ActorInit(nullptr, TEXT("ComplexActor"), AActor::GetStaticClass());
    AActor* TestActor = static_cast< AActor* >(ActorInit.CreateObject());

    if (TestActor)
    {
        std::cout << "Complex Actor Creation Success\n";
        std::cout << "  Actor Name: " << TestActor->GetName().c_str() << "\n";

        // Component creation test
        USceneComponent* NewSceneComponent = TestActor->CreateComponent<USceneComponent>(TEXT("CustomSceneComponent"));
        UActorComponent* NewActorComponent = TestActor->CreateComponent<UActorComponent>(TEXT("CustomActorComponent"));

        if (NewSceneComponent && NewActorComponent)
        {
            std::cout << "Component Creation Success\n";
            std::cout << "  SceneComponent: " << NewSceneComponent->GetName().c_str() << "\n";
            std::cout << "  ActorComponent: " << NewActorComponent->GetName().c_str() << "\n";

            // Check component count
            const TArray<UActorComponent*>& Components = TestActor->GetComponents();
            std::cout << "  Total Components: " << Components.size() << "\n";

            // Transform test
            TestActor->SetActorLocation(FVector(100.0f, 200.0f, 300.0f));
            FVector ActorLocation = TestActor->GetActorLocation();
            std::cout << "  Actor Location: (" << ActorLocation.X << ", " << ActorLocation.Y << ", " << ActorLocation.Z << ")\n";
        }

        delete TestActor;
        std::cout << "Complex Actor Deletion Completed\n";
    }
}

void TestGarbageCollectionIntegration()
{
    std::cout << "\n=== Testing GC Integration ===\n";

    int32 InitialObjectCount = GUObjectArray.GetObjectArrayNum();
    std::cout << "Initial Object Count: " << InitialObjectCount << "\n";

    // Create multiple objects
    TArray<UObject*> TestObjects;
    for (int32 i = 0; i < 10; ++i)
    {
        FString ObjectName = FString("TestObject_") + std::to_string(i).c_str();
        UObject* TestObj = NewObject<UObject>(nullptr, ObjectName);
        TestObjects.push_back(TestObj);
    }

    int32 AfterCreationCount = GUObjectArray.GetObjectArrayNum();
    std::cout << "After Creation Count: " << AfterCreationCount << "\n";
    std::cout << "Created Objects: " << (AfterCreationCount - InitialObjectCount) << "\n";

    // Mark some objects as garbage
    for (int32 i = 0; i < 5; ++i)
    {
        GUObjectArray.MarkAsGarbage(TestObjects[ i ]);
    }

    // Execute garbage collection
    GUObjectArray.PerformGarbageCollector();

    int32 AfterGCCount = GUObjectArray.GetObjectArrayNum();
    std::cout << "After GC Count: " << AfterGCCount << "\n";
    std::cout << "Collected Objects: " << (AfterCreationCount - AfterGCCount) << "\n";

    // Clean up remaining objects
    for (int32 i = 5; i < TestObjects.size(); ++i)
    {
        delete TestObjects[ i ];
    }

    std::cout << "GC Integration Test Completed\n";
}

void TestGlobalCreateDefaultSubobject()
{
    std::cout << "\n=== Testing Global CreateDefaultSubobject ===\n";

    // Create parent actor
    AActor* ParentActor = NewObject<AActor>(nullptr, TEXT("ParentActor"));
    if (ParentActor)
    {
        std::cout << "Parent Actor Created: " << ParentActor->GetName().c_str() << "\n";

        // Test 1: Basic CreateDefaultSubobject
        USceneComponent* SceneComp1 = CreateDefaultSubobject<USceneComponent>(ParentActor, TEXT("GlobalSceneComponent"));
        if (SceneComp1)
        {
            std::cout << "  Global CreateDefaultSubobject Success\n";
            std::cout << "    Component Name: " << SceneComp1->GetName().c_str() << "\n";
            std::cout << "    Component Class: " << SceneComp1->GetClassName().c_str() << "\n";
            std::cout << "    Owner: " << (SceneComp1->GetOuter() == ParentActor ? "Correct" : "Wrong") << "\n";
        }

        // Test 2: Auto-name version
        UActorComponent* ActorComp1 = CreateDefaultSubobject<UActorComponent>(ParentActor);
        if (ActorComp1)
        {
            std::cout << "  Auto-name CreateDefaultSubobject Success\n";
            std::cout << "    Component Name: " << ActorComp1->GetName().c_str() << "\n";
            std::cout << "    Component Class: " << ActorComp1->GetClassName().c_str() << "\n";
        }

        // Test 3: Custom class version
        USceneComponent* SceneComp2 = CreateDefaultSubobject<USceneComponent>(
            ParentActor, TEXT("CustomClassComponent"), USceneComponent::GetStaticClass());
        if (SceneComp2)
        {
            std::cout << "  Custom Class CreateDefaultSubobject Success\n";
            std::cout << "    Component Name: " << SceneComp2->GetName().c_str() << "\n";
        }

        // Check component count
        const TArray<UActorComponent*>& Components = ParentActor->GetComponents();
        std::cout << "  Total Components in Actor: " << Components.size() << "\n";

        delete ParentActor;
        std::cout << "Global CreateDefaultSubobject Test Completed\n";
    }
}

void TestCustomActor()
{
    std::cout << "\n=== Testing Custom Actor ===\n";

    // Create custom actor
    ATestActor* TestActor = NewObject<ATestActor>(nullptr, TEXT("MyCustomActor"));
    if (TestActor)
    {
        std::cout << "Custom Actor Creation Success\n";
        std::cout << "  Actor Name: " << TestActor->GetName().c_str() << "\n";
        std::cout << "  Class Name: " << TestActor->GetClassName().c_str() << "\n";
        std::cout << "  RTTI Test: " << (TestActor->IsA<AActor>() ? "AActor inheritance confirmed" : "Inheritance failed") << "\n";
        std::cout << "  RTTI Test: " << (TestActor->IsA<ATestActor>() ? "ATestActor type confirmed" : "Type failed") << "\n";

        delete TestActor;
        std::cout << "Custom Actor Deletion Completed\n";
    }
}

// Main test function
void RunObjectInitializerTests()
{
    // Enable console window
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout << "========================================\n";
    std::cout << "  FObjectInitializer & NewObject Tests\n";
    std::cout << "========================================\n";

    TestBasicObjectInitializer();
    TestNewObjectFunction();
    TestActorWithComponents();
    TestGlobalCreateDefaultSubobject();
    TestGarbageCollectionIntegration();
    TestCustomActor();

    std::cout << "\n========================================\n";
    std::cout << "       All Tests Completed\n";
    std::cout << "========================================\n";
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

    RunObjectInitializerTests();

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
