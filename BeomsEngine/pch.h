// pch.h - �̸� �����ϵ� ��� ����
// ��� .cpp ���Ͽ��� #include "pch.h"�� ù ��° ���ο� �־�� �մϴ�.
// PCH�� ���Ե� ����� �����ϸ� ��ü ������Ʈ�� �������ϵ�
// ���� PCH ���Ͽ��� ���� ����ϰ�, ���� ������� �ʴ� ����� ���Խ��Ѿ� ��
#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define NOMINMAX

// Windows ��� ����
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

// C ��Ÿ�� ��� ����
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>

// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D ��뿡 �ʿ��� ������ϵ��� ����
#include <d3d11.h>
#include <d3dcompiler.h>

// ImGui
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

// �⺻ Ÿ�� ����
typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

// STL �����̳ʿ� ���� ��Ī ����
template <typename T> using TArray = std::vector<T>;
template <typename T> using TLinkedList = std::list<T>;
template <typename T> using TSet = std::unordered_set<T>;
template <typename K, typename V> using TMap = std::unordered_map<K, V>;
template <typename K, typename V> using TPair = std::pair<K, V>;
template <typename T, unsigned N> using TStaticArray = std::array<T, N>;
template <typename T> using TQueue = std::queue<T>;
using FString = std::string;