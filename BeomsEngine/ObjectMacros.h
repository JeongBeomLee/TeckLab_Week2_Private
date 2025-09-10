#pragma once
#include "Class.h"

// RTTI 매크로 시스템

// UCLASS 매크로 - 클래스 선언 후에 사용
#define UCLASS() \
    public: \
        static UClass* GetStaticClass(); \
        virtual UClass* GetClass() const override; \
    private:

// GENERATED_BODY 매크로 - 클래스 내부에서 사용  
#define GENERATED_BODY(ClassName, SuperClassName) \
    public: \
        typedef ClassName ThisClass; \
        typedef SuperClassName Super; \
        static UObject* CreateInstance() { return new ClassName(); } \
    private:

// IMPLEMENT_CLASS 매크로 - cpp 파일에서 사용
#define IMPLEMENT_CLASS(ClassName, SuperClassName) \
    UClass* ClassName::GetStaticClass() \
    { \
        static UClass* StaticClass = nullptr; \
        if (!StaticClass) \
        { \
            StaticClass = new UClass( \
                TEXT(#ClassName), \
                SuperClassName::GetStaticClass(), \
                &ClassName::CreateInstance \
            ); \
            UClass::RegisterClass(StaticClass); \
        } \
        return StaticClass; \
    } \
    UClass* ClassName::GetClass() const \
    { \
        return ClassName::GetStaticClass(); \
    }

// 루트 클래스용 특별 매크로 (SuperClass가 없는 경우)
#define IMPLEMENT_ROOT_CLASS(ClassName) \
    UClass* ClassName::GetStaticClass() \
    { \
        static UClass* StaticClass = nullptr; \
        if (!StaticClass) \
        { \
            StaticClass = new UClass( \
                TEXT(#ClassName), \
                nullptr, \
                &ClassName::CreateInstance \
            ); \
            UClass::RegisterClass(StaticClass); \
        } \
        return StaticClass; \
    } \
    UClass* ClassName::GetClass() const \
    { \
        return ClassName::GetStaticClass(); \
    }

// 타입 캐스팅 매크로
#define CAST(TargetType, SourceObject) \
    Cast<TargetType>(SourceObject)

// 안전한 캐스팅 (nullptr 체크 포함)
#define SAFE_CAST(TargetType, SourceObject) \
    (SourceObject ? Cast<TargetType>(SourceObject) : nullptr)

// 클래스 타입 체크 매크로
#define IS_A(Object, TargetClass) \
    (Object && Object->IsA(TargetClass::GetStaticClass()))

// 유효한 오브젝트 체크
#define IS_VALID(Object) \
    (Object && Object->IsValid())

// 템플릿 캐스트 함수
template<typename T>
T* Cast(UObject* Object)
{
    if (!Object)
        return nullptr;
        
    if (Object->IsA(T::GetStaticClass()))
    {
        return static_cast<T*>(Object);
    }
    
    return nullptr;
}

// const 버전 캐스트
template<typename T>
const T* Cast(const UObject* Object)
{
    if (!Object)
        return nullptr;
        
    if (Object->IsA(T::GetStaticClass()))
    {
        return static_cast<const T*>(Object);
    }
    
    return nullptr;
}