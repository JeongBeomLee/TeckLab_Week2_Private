#pragma once
#include <cctype>

// FName의 내부 표현을 위한 타입들
using FNameEntryId = uint32;
using FNameDisplayIndex = uint32;

// 유효하지 않은 FName을 나타내는 상수
constexpr FNameEntryId NAME_None = 0;

struct FNameEntry
{
    FString ComparisonString;  // 소문자로 저장된 비교용 문자열
    TArray<FString> DisplayVariations;  // 원본 대소문자 변형들

    FNameEntry() = default;
    FNameEntry(const FString& InComparison, const FString& InDisplay)
        : ComparisonString(InComparison)
    {
        DisplayVariations.push_back(InDisplay);
    }
};

class FNamePool
{
private:
    // FName 엔트리들 (인덱스 = FNameEntryId)
    TArray<FNameEntry> NameEntries;

    // 소문자 문자열 -> 인덱스 매핑을 위한 해시맵
    std::unordered_map<FString, FNameEntryId> StringToIdMap;

    // 다음 할당할 ID
    FNameEntryId NextId;

    // 싱글톤 인스턴스
    static FNamePool* Instance;

    FNamePool()
        : NextId(1) // 0은 NAME_None으로 예약
    {
        // NAME_None (빈 문자열) 추가
        NameEntries.push_back(FNameEntry("", ""));
        StringToIdMap[""] = NAME_None;
    }

    // 문자열을 소문자로 변환하는 헬퍼 함수
    static FString ToLowerCase(const FString& InString)
    {
        FString Result = InString;
        std::transform(Result.begin(), Result.end(), Result.begin(), ::tolower);
        return Result;
    }

public:
    // 싱글톤 인스턴스 얻기
    static FNamePool& GetInstance()
    {
        if (!Instance)
        {
            Instance = new FNamePool();
        }
        return *Instance;
    }

    // 싱글톤 인스턴스 해제
	static void DistroyInstance()
	{
		if (Instance)
		{
			delete Instance;
			Instance = nullptr;
		}
	}

    // 문자열을 FName ID와 DisplayIndex로 변환 (없으면 새로 생성)
    std::pair<FNameEntryId, FNameDisplayIndex> Store(const FString& InString)
    {
        // 대소문자 무시를 위해 소문자로 변환하여 검색
        FString LowerCaseString = ToLowerCase(InString);

        // 이미 존재하는지 확인 (소문자로)
        auto It = StringToIdMap.find(LowerCaseString);
        if (It != StringToIdMap.end())
        {
            FNameEntryId ExistingId = It->second;
            FNameEntry& Entry = NameEntries[ExistingId];

            // 기존 Display 변형들에서 정확히 일치하는 것 찾기
            for (size_t i = 0; i < Entry.DisplayVariations.size(); ++i)
            {
                if (Entry.DisplayVariations[i] == InString)
                {
                    return std::make_pair(ExistingId, static_cast<FNameDisplayIndex>(i));
                }
            }

            // 새로운 Display 변형 추가
            Entry.DisplayVariations.push_back(InString);
            return std::make_pair(ExistingId, static_cast<FNameDisplayIndex>(Entry.DisplayVariations.size() - 1));
        }

        // 완전히 새로운 엔트리 생성
        FNameEntryId NewId = NextId++;
        NameEntries.push_back(FNameEntry(LowerCaseString, InString));
        StringToIdMap[LowerCaseString] = NewId;

        return std::make_pair(NewId, 0); // 첫 번째 변형이므로 DisplayIndex = 0
    }

    // FName ID와 DisplayIndex를 문자열로 변환
    const FString& Resolve(FNameEntryId Id, FNameDisplayIndex DisplayIndex = 0) const
    {
        if (Id < NameEntries.size())
        {
            const FNameEntry& Entry = NameEntries[Id];
            if (DisplayIndex < Entry.DisplayVariations.size())
            {
                return Entry.DisplayVariations[DisplayIndex];
            }
            // DisplayIndex가 범위를 벗어나면 첫 번째 변형 반환
            if (!Entry.DisplayVariations.empty())
            {
                return Entry.DisplayVariations[0];
            }
        }

        // 잘못된 ID인 경우 빈 문자열 반환
        static const FString EmptyString = "";
        return EmptyString;
    }

    // 비교용 문자열 반환 (소문자)
    const FString& ResolveComparison(FNameEntryId Id) const
    {
        if (Id < NameEntries.size())
        {
            return NameEntries[Id].ComparisonString;
        }

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
            TotalSize += Entry.ComparisonString.size();
            for (const auto& DisplayVar : Entry.DisplayVariations)
            {
                TotalSize += DisplayVar.size();
            }
        }
        return TotalSize;
    }
};

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
    {
        if (InName.empty())
        {
            ComparisonIndex = NAME_None;
            DisplayIndex = 0;
        }
        else
        {
            // 전역 풀에 저장하고 ID와 DisplayIndex 받기
            auto Result = FNamePool::GetInstance().Store(InName);
            ComparisonIndex = Result.first;
            DisplayIndex = Result.second;
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

    // 문자열 변환 (원본 대소문자 보존)
    FString ToString() const
    {
        return FNamePool::GetInstance().Resolve(ComparisonIndex, DisplayIndex);
    }

    const char* ToConstCharPointer() const
    {
        static FString CachedString = ToString();
        return CachedString.c_str();
    }

    // 비교용 문자열 반환 (소문자)
    FString ToComparisonString() const
    {
        return FNamePool::GetInstance().ResolveComparison(ComparisonIndex);
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

    // 정적 상수
    static const FName None;
};

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

// 자주 사용될 것 같은 FName들을 미리 정의
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