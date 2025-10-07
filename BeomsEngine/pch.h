// pch.h - 미리 컴파일된 헤더 파일
// 모든 .cpp 파일에서 #include "pch.h"가 첫 번째 라인에 있어야 합니다.
// PCH에 포함된 헤더를 수정하면 전체 프로젝트가 재컴파일됨
// 따라서 PCH 파일에는 자주 사용하고, 자주 변경되지 않는 헤더만 포함시켜야 함
#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Windows 헤더 파일
#include <windows.h>

// STL
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
#include <dxgi.h>
#include <wrl.h>

// ImGui
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

// === Core Types ===
#include "Types.h"
#include "String.h"
#include "Containers.h"
#include "Name.h"

// === Templates ===
#include "UniquePointer.h"
#include "SharedPointer.h"
#include "WeakPointer.h"

// === Object System ===
#include "Object.h"
#include "ObjectInitializer.h"
#include "ObjectMacros.h"
#include "Class.h"
#include "UObjectArray.h"
#include "UObjectIterator.h"

// === Math ===
#include "Vector.h"
#include "Vector2.h"
#include "Matrix.h"

// === Standard Library ===
#include <unordered_map>
#include <cassert>