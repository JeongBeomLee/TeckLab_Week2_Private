#include "pch.h"
#include "Name.h"

// FNamePool 싱글톤 인스턴스 정의
FNamePool* FNamePool::Instance = nullptr;

// 상수 정의
const FName FName::None = FName();

// EName 네임스페이스 상수들 정의
namespace EName
{
    // 게임 오브젝트 관련
    const FName None = FName();
    const FName Default = FName("Default");
    const FName Root = FName("Root");
    const FName World = FName("World");

    // 컴포넌트 관련
    const FName Transform = FName("Transform");
    const FName Mesh = FName("Mesh");
    const FName Collision = FName("Collision");
    const FName Physics = FName("Physics");

    // 머티리얼 관련
    const FName Material = FName("Material");
    const FName Texture = FName("Texture");
    const FName BaseColor = FName("BaseColor");
    const FName Normal = FName("Normal");
    const FName Roughness = FName("Roughness");
    const FName Metallic = FName("Metallic");

    // 애니메이션 관련
    const FName Bone = FName("Bone");
    const FName Socket = FName("Socket");
    const FName Animation = FName("Animation");

    // 이벤트 관련
    const FName BeginPlay = FName("BeginPlay");
    const FName EndPlay = FName("EndPlay");
    const FName Tick = FName("Tick");
    const FName Update = FName("Update");

    // 입력 관련
    const FName Input = FName("Input");
    const FName Action = FName("Action");
    const FName Axis = FName("Axis");
}