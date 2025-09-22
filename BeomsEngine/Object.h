#pragma once
#include "Types.h"
#include "String.h"
#include "Array.h"
#include "Name.h"

#ifdef GetClassName
#undef GetClassName
#endif

// 전방 선언
class UClass;
class FUObjectArray;
class FObjectInitializer;

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
    FName GetName() const { return ObjectName; }
    void SetName(const FName& InName) { ObjectName = InName; }
    FString GetNameString() const { return ObjectName.ToString(); }
    
    uint64 GetUniqueID() const { return UniqueID; }
    
    // 클래스 정보 및 RTTI
    virtual FString GetClassName() const { return "UObject"; }
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
    
    // GUObjectArray 관리
    int32 GetInternalIndex() const { return InternalIndex; }
    void SetInternalIndex(int32 Index) { InternalIndex = Index; }
    
protected:
    // 오브젝트 상태
    bool bIsValid;
    bool bPendingKill;
    bool bHasBegunPlay;
    
    // 오브젝트 정보
    FName ObjectName;
    uint64 UniqueID;
    UObject* Outer;
    
    // GUObjectArray에서의 인덱스
    int32 InternalIndex;
    
    // 정적 ID 생성기
    static uint64 GenerateUniqueID();
    
private:
    static uint64 NextUniqueID;
};

template<typename T>
bool UObject::IsA() const
{
    return IsA(T::GetStaticClass());
}