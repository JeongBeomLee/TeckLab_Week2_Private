#pragma once
#include "Types.h"
#include "Containers.h"

// FName의 내부 표현을 위한 타입들
using FNameEntryId = uint32;
using FNameDisplayIndex = uint32;

// 유효하지 않은 FName을 나타내는 상수
constexpr FNameEntryId NAME_None = 0;

/**
 * FName 시스템의 핵심: 전역 문자열 테이블
 * 모든 FName 문자열을 단 한 번만 저장하는 싱글톤
 */
class FNamePool
{
private:
    // 문자열 데이터를 저장하는 배열 (인덱스 = FNameEntryId)
    TArray<FString> NameEntries;

    // 문자열 -> 인덱스 매핑을 위한 해시맵 (빠른 검색)
    std::unordered_map<FString, FNameEntryId> StringToIdMap;

    // 다음 할당할 ID
    FNameEntryId NextId;

    // 싱글톤 인스턴스
    static FNamePool* Instance;

    FNamePool()
        : NextId(1) // 0은 NAME_None으로 예약
    {
        // NAME_None (빈 문자열) 추가
        NameEntries.push_back("");
        StringToIdMap[""] = NAME_None;
    }

public:
    // 싱글톤 인스턴스 얻기
    static FNamePool& Get()
    {
        if (!Instance)
        {
            Instance = new FNamePool();
        }
        return *Instance;
    }

    // 문자열을 FName ID로 변환 (없으면 새로 생성)
    FNameEntryId Store(const FString& InString)
    {
        // 이미 존재하는지 확인
        auto It = StringToIdMap.find(InString);
        if (It != StringToIdMap.end())
        {
            return It->second; // 기존 ID 반환
        }

        // 새로운 엔트리 생성
        FNameEntryId NewId = NextId++;
        NameEntries.push_back(InString);
        StringToIdMap[InString] = NewId;

        return NewId;
    }

    // FName ID를 문자열로 변환
    const FString& Resolve(FNameEntryId Id) const
    {
        if (Id < NameEntries.size())
        {
            return NameEntries[Id];
        }

        // 잘못된 ID인 경우 빈 문자열 반환
        static const FString EmptyString = "";
        return EmptyString;
    }

    // 통계 정보
    size_t GetNumEntries() const
    {
        return NameEntries.size();
    }

    size_t GetMemoryUsage() const
    {
        size_t TotalSize = 0;
        for (const auto& Entry : NameEntries)
        {
            TotalSize += Entry.size();
        }
        return TotalSize;
    }

    // 디버깅용: 모든 이름 출력
    void DumpAllNames() const
    {
        for (size_t i = 0; i < NameEntries.size(); ++i)
        {
            printf("ID %zu: '%s'\n", i, NameEntries[i].c_str());
        }
    }
};

/**
 * FName: 고성능 불변 문자열 식별자
 *
 * 핵심 특징:
 * - 메모리 효율적: 동일한 문자열은 전역에서 단 한 번만 저장
 * - 빠른 비교: 정수 비교와 동일한 성능
 * - 불변성: 한번 생성되면 내용 변경 불가
 * - 스레드 안전: 멀티스레드 환경에서 안전하게 사용 가능
 */
class FName
{
private:
    // 전역 문자열 테이블의 인덱스
    FNameEntryId ComparisonIndex;

    // 표시용 인덱스 (대소문자 구분, 번호 등)
    FNameDisplayIndex DisplayIndex;

public:
    // 기본 생성자 (NAME_None)
    FName()
        : ComparisonIndex(NAME_None)
        , DisplayIndex(0)
    {}

    // 문자열로부터 생성
    explicit FName(const FString& InName)
        : DisplayIndex(0)
    {
        if (InName.empty())
        {
            ComparisonIndex = NAME_None;
        }
        else
        {
            // 전역 풀에 저장하고 ID 받기
            ComparisonIndex = FNamePool::Get().Store(InName);
        }
    }

    // C 스타일 문자열로부터 생성
    explicit FName(const char* InName)
        : FName(FString(InName ? InName : ""))
    {}

    // 복사 생성자
    FName(const FName& Other)
        : ComparisonIndex(Other.ComparisonIndex)
        , DisplayIndex(Other.DisplayIndex)
    {}

    // 대입 연산자
    FName& operator=(const FName& Other)
    {
        ComparisonIndex = Other.ComparisonIndex;
        DisplayIndex = Other.DisplayIndex;
        return *this;
    }

    // 비교 연산자들 (매우 빠름 - 정수 비교)
    bool operator==(const FName& Other) const
    {
        return ComparisonIndex == Other.ComparisonIndex;
    }

    bool operator!=(const FName& Other) const
    {
        return ComparisonIndex != Other.ComparisonIndex;
    }

    bool operator<(const FName& Other) const
    {
        return ComparisonIndex < Other.ComparisonIndex;
    }

    bool operator<=(const FName& Other) const
    {
        return ComparisonIndex <= Other.ComparisonIndex;
    }

    bool operator>(const FName& Other) const
    {
        return ComparisonIndex > Other.ComparisonIndex;
    }

    bool operator>=(const FName& Other) const
    {
        return ComparisonIndex >= Other.ComparisonIndex;
    }

    // 문자열 변환
    FString ToString() const
    {
        return FNamePool::Get().Resolve(ComparisonIndex);
    }

    const char* GetPlainNameString() const
    {
        static FString CachedString = ToString();
        return CachedString.c_str();
    }

    // 유효성 검사
    bool IsValid() const
    {
        return ComparisonIndex != NAME_None;
    }

    bool IsNone() const
    {
        return ComparisonIndex == NAME_None;
    }

    // 내부 ID 접근 (디버깅용)
    FNameEntryId GetComparisonIndex() const
    {
        return ComparisonIndex;
    }

    FNameDisplayIndex GetDisplayIndex() const
    {
        return DisplayIndex;
    }

    // 해시 함수 (컨테이너에서 사용)
    size_t GetHash() const
    {
        return static_cast<size_t>(ComparisonIndex);
    }

    // 정적 상수들
    static const FName None;
};

// FNamePool 싱글톤 인스턴스는 cpp 파일에서 정의해야 함

// 상수 정의는 cpp 파일에서 처리

// 해시 함수 특수화 (std::unordered_map 등에서 사용)
namespace std
{
    template<>
    struct hash<FName>
    {
        size_t operator()(const FName& Name) const
        {
            return Name.GetHash();
        }
    };
}

// 유용한 매크로들
#define NAME_None FName()

// 자주 사용되는 FName들을 미리 정의
namespace EName
{
    // 게임 오브젝트 관련
    extern const FName None;
    extern const FName Default;
    extern const FName Root;
    extern const FName World;

    // 컴포넌트 관련
    extern const FName Transform;
    extern const FName Mesh;
    extern const FName Collision;
    extern const FName Physics;

    // 머티리얼 관련
    extern const FName Material;
    extern const FName Texture;
    extern const FName BaseColor;
    extern const FName Normal;
    extern const FName Roughness;
    extern const FName Metallic;

    // 애니메이션 관련
    extern const FName Bone;
    extern const FName Socket;
    extern const FName Animation;

    // 이벤트 관련
    extern const FName BeginPlay;
    extern const FName EndPlay;
    extern const FName Tick;
    extern const FName Update;

    // 입력 관련
    extern const FName Input;
    extern const FName Action;
    extern const FName Axis;
}

// 사용 예시 및 테스트 함수들
//namespace FNameTest
//{
//    void RunTests()
//    {
//        printf("=== FName System Tests ===\n");
//
//        // 1. 기본 생성 및 비교 테스트
//        FName Name1("TestName");
//        FName Name2("TestName");
//        FName Name3("DifferentName");
//
//        printf("Same string comparison: %s\n", (Name1 == Name2) ? "PASS" : "FAIL");
//        printf("Different string comparison: %s\n", (Name1 != Name3) ? "PASS" : "FAIL");
//
//        // 2. 메모리 효율성 테스트
//        TArray<FName> Names;
//        for (int i = 0; i < 1000; ++i)
//        {
//            Names.push_back(FName("RepeatedName")); // 같은 문자열 1000번
//        }
//
//        printf("Created 1000 FNames with same string\n");
//        printf("Total unique entries in pool: %zu\n", FNamePool::Get().GetNumEntries());
//        printf("Memory usage: %zu bytes\n", FNamePool::Get().GetMemoryUsage());
//
//        // 3. 성능 테스트
//        auto Start = std::chrono::high_resolution_clock::now();
//
//        for (int i = 0; i < 100000; ++i)
//        {
//            bool Result = Name1 == Name2; // FName 비교
//            (void)Result;
//        }
//
//        auto End = std::chrono::high_resolution_clock::now();
//        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(End - Start);
//
//        printf("100,000 FName comparisons took: %lld microseconds\n", Duration.count());
//
//        // 4. 문자열 변환 테스트
//        printf("Name1 toString: '%s'\n", Name1.ToString().c_str());
//        printf("Name3 toString: '%s'\n", Name3.ToString().c_str());
//
//        // 5. 미리 정의된 이름들 테스트
//        printf("EName::Material: '%s'\n", EName::Material.ToString().c_str());
//        printf("EName::BeginPlay: '%s'\n", EName::BeginPlay.ToString().c_str());
//
//        printf("=== FName Tests Complete ===\n\n");
//    }
//}

/**
 * 실제 사용 예시:
 *
 * // 1. 기본 사용
 * FName ComponentName("MeshComponent");
 * FName MaterialSlot("DefaultMaterial");
 *
 * // 2. 빠른 비교
 * if (ComponentName == EName::Mesh)
 * {
 *     // 매우 빠른 정수 비교
 * }
 *
 * // 3. 컨테이너의 키로 사용
 * std::unordered_map<FName, UComponent*> ComponentMap;
 * ComponentMap[EName::Transform] = TransformComponent;
 *
 * // 4. 문자열 변환
 * FString DisplayName = ComponentName.ToString();
 * printf("Component: %s\n", ComponentName.GetPlainNameString());
 */