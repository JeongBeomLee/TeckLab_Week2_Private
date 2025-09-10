#pragma once
#include "Types.h"
#include "FString.h"
#include "Array.h"
#include "Map.h"

#ifdef RegisterClass
#undef RegisterClass
#endif

// 전방 선언
class UObject;
class UClass;

// 클래스 생성자 함수 포인터 타입
typedef UObject* (*ClassConstructorType)();

// UClass - 런타임 타입 정보를 담는 메타 클래스
class UClass
{
public:
    UClass(const FString& InClassName, UClass* InSuperClass, ClassConstructorType InConstructor);
    ~UClass();

    // 클래스 정보
    const FString& GetName() const { return ClassName; }
    UClass* GetSuperClass() const { return SuperClass; }
    
    // 상속 관계 확인
    bool IsChildOf(const UClass* SomeBase) const;
    bool IsChildOf(const FString& BaseClassName) const;
    
    // 오브젝트 생성
    UObject* CreateDefaultObject() const;
    
    // 클래스 등록 시스템
    static void RegisterClass(UClass* NewClass);
    static UClass* FindClass(const FString& ClassName);
    static const TArray<UClass*>& GetAllClasses() { return RegisteredClasses; }
    
    // 클래스 계층구조 정보
    void GetAllChildClasses(TArray<UClass*>& OutChildClasses) const;
    bool HasChildren() const;
    
private:
    FString ClassName;
    UClass* SuperClass;
    ClassConstructorType Constructor;
    
    // 전역 클래스 레지스트리
    static TArray<UClass*> RegisteredClasses;
    static TMap<FString, UClass*> ClassMap;
    
    // 자식 클래스들 (캐시)
    mutable TArray<UClass*> ChildClasses;
    mutable bool bChildClassesCached;
    
    void CacheChildClasses() const;
};