// pch.h - 미리 컴파일된 헤더 파일
// 모든 .cpp 파일에서 #include "pch.h"가 첫 번째 라인에 있어야 합니다.
// PCH에 포함된 헤더를 수정하면 전체 프로젝트가 재컴파일됨
// 따라서 PCH 파일에는 자주 사용하고, 자주 변경되지 않는 헤더만 포함시켜야 함
#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NOMINMAX

// Windows 헤더 파일
#include <windows.h>

// STL
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <queue>
#include <algorithm>

// C 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>

// D3D 사용에 필요한 라이브러리들을 링크
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D 사용에 필요한 헤더파일들을 포함
#include <d3d11.h>
#include <d3dcompiler.h>

// ImGui
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

// 기본 타입 정의
typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

// STL 컨테이너에 대한 별칭 정의
template <typename T> using TArray = std::vector<T>;
template <typename T> using TLinkedList = std::list<T>;
template <typename T> using TSet = std::unordered_set<T>;
template <typename K, typename V> using TMap = std::unordered_map<K, V>;
template <typename K, typename V> using TPair = std::pair<K, V>;
template <typename T, unsigned N> using TStaticArray = std::array<T, N>;
template <typename T> using TQueue = std::queue<T>;
using FString = std::string;