// BeomsEngine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "BeomsEngine.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "SceneComponent.h"
#include "UObjectArray.h"
#include "Math.h"
#include <iostream>

void TestGUObjectArray()
{
    // 콘솔 창 활성화
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    
    std::cout << "=== GUObjectArray System Test ===" << std::endl;
    
    // 1. 초기 상태 확인
    std::cout << "\n--- Initial State ---" << std::endl;
    std::cout << "Objects in array: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    std::cout << "Max objects ever: " << GUObjectArray.GetMaxObjectsEver() << std::endl;
    
    // 2. UObject 생성 테스트
    std::cout << "\n--- Creating UObjects ---" << std::endl;
    UObject* obj1 = new UObject();
    obj1->SetName(TEXT("TestObject1"));
    
    UObject* obj2 = new UObject();
    obj2->SetName(TEXT("TestObject2"));
    
    AActor* actor1 = new AActor();
    actor1->SetName(TEXT("TestActor1"));
    
    std::cout << "Created 3 objects (2 UObjects, 1 AActor)" << std::endl;
    std::cout << "Objects in array: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    std::cout << "Max objects ever: " << GUObjectArray.GetMaxObjectsEver() << std::endl;
    
    // 3. 모든 객체 검색 테스트
    std::cout << "\n--- Getting All Objects ---" << std::endl;
    TArray<UObject*> allObjects;
    GUObjectArray.GetAllObjects(allObjects);
    
    std::cout << "Found " << allObjects.size() << " valid objects:" << std::endl;
    for (int32 i = 0; i < allObjects.size(); ++i)
    {
        UObject* obj = allObjects[i];
        if (obj)
        {
            std::cout << "  [" << i << "] " << obj->GetName().c_str() 
                     << " (Type: " << obj->GetClassName().c_str() 
                     << ", Index: " << obj->GetInternalIndex() << ")" << std::endl;
        }
    }
    
    // 4. 클래스별 객체 검색 테스트
    std::cout << "\n--- Getting Objects by Class ---" << std::endl;
    TArray<UObject*> baseObjects;
    TArray<AActor*> actorObjects;
    
    GUObjectArray.GetObjectsOfClass<UObject>(baseObjects);
    GUObjectArray.GetObjectsOfClass<AActor>(actorObjects);
    
    std::cout << "UObject instances: " << baseObjects.size() << std::endl;
    std::cout << "AActor instances: " << actorObjects.size() << std::endl;
    
    // 5. 객체 삭제 테스트
    std::cout << "\n--- Deleting Objects ---" << std::endl;
    std::cout << "Deleting obj1..." << std::endl;
    delete obj1;
    
    std::cout << "Objects in array after deletion: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    
    // 6. Garbage Collection 테스트
    std::cout << "\n--- Garbage Collection Test ---" << std::endl;
    obj2->MarkPendingKill();
    std::cout << "Marked obj2 as PendingKill" << std::endl;
    
    std::cout << "Before GC - Objects in array: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    GUObjectArray.PerformGarbageCollection();
    std::cout << "After GC - Objects in array: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    
    // 7. 남은 객체 확인
    std::cout << "\n--- Remaining Objects ---" << std::endl;
    allObjects.clear();
    GUObjectArray.GetAllObjects(allObjects);
    
    std::cout << "Remaining objects: " << allObjects.size() << std::endl;
    for (int32 i = 0; i < allObjects.size(); ++i)
    {
        UObject* obj = allObjects[i];
        if (obj)
        {
            std::cout << "  [" << i << "] " << obj->GetName().c_str() 
                     << " (Type: " << obj->GetClassName().c_str() << ")" << std::endl;
        }
    }
    
    // 8. 정리
    std::cout << "\n--- Cleanup ---" << std::endl;
    delete actor1;
    
    std::cout << "Final objects in array: " << GUObjectArray.GetObjectArrayNum() << std::endl;
    std::cout << "Final max objects ever: " << GUObjectArray.GetMaxObjectsEver() << std::endl;
    
    std::cout << "\n=== GUObjectArray System Test Complete ===" << std::endl;
    
    // 콘솔 창 닫기
    if (fp != nullptr) fclose(fp);
    FreeConsole();
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

	// GUObjectArray 시스템 테스트
	TestGUObjectArray();

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
