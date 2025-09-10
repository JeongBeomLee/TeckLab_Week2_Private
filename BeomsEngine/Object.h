#pragma once
#include "Types.h"
#include "String.h"
#include "Array.h"

#ifdef GetClassName
#undef GetClassName
#endif

// 전방 선언
class UClass;

// UObject 기본 클래스
class UObject
{
public:
    UObject();
    virtual ~UObject();

    // 기본 UObject 인터페이스
    virtual void BeginPlay() {}
    virtual void EndPlay() {}
    virtual void Tick(float DeltaTime) {}

    // 오브젝트 정보
    FString GetName() const { return ObjectName; }
    void SetName(const FString& InName) { ObjectName = InName; }
    
    uint64 GetUniqueID() const { return UniqueID; }
    
    // 클래스 정보 및 RTTI
    virtual FString GetClassName() const { return TEXT("UObject"); }
    virtual UClass* GetClass() const;
    static UClass* GetStaticClass();
    
    // RTTI
    bool IsA(const UClass* SomeClass) const;
    bool IsA(const FString& ClassName) const;
    
    template<typename T>
    bool IsA() const;
    
    static UObject* CreateInstance() { return new UObject(); }
    
    // 소유자 관계
    UObject* GetOuter() const { return Outer; }
    void SetOuter(UObject* InOuter) { Outer = InOuter; }
    
    // 유효성 검사
    bool IsValid() const { return bIsValid && !bPendingKill; }
    void MarkPendingKill() { bPendingKill = true; }
    bool IsPendingKill() const { return bPendingKill; }
    
    // 리플렉션을 위한 기본 구조
    virtual void GetProperties(TArray<FString>& OutProperties) const {}
    
    // 직렬화
    virtual void Serialize() {}
    virtual void Deserialize() {}
    
protected:
    // 오브젝트 상태
    bool bIsValid;
    bool bPendingKill;
    bool bHasBegunPlay;
    
    // 오브젝트 정보
    FString ObjectName;
    uint64 UniqueID;
    UObject* Outer;
    
    // 정적 ID 생성기
    static uint64 GenerateUniqueID();
    
private:
    static uint64 NextUniqueID;
};