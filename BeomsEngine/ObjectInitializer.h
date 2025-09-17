#pragma once
#include "Types.h"
#include "String.h"

// 전방 선언
class UObject;
class UClass;

// FObjectInitializer - UObject 생성 시 초기화 정보를 담는 구조체
class FObjectInitializer
{
public:
    // 생성자
    explicit FObjectInitializer(UObject* InOuter = nullptr, const FString& InName = FString(), UClass* InClass = nullptr);

    // 복사 생성자
    FObjectInitializer(const FObjectInitializer& Other);

    // 대입 연산자
    FObjectInitializer& operator=(const FObjectInitializer& Other);

    // 소멸자
    ~FObjectInitializer();

    // 초기화 정보 접근자
    UObject* GetOuter() const { return Outer; }
    const FString& GetName() const { return ObjectName; }
    UClass* GetClass() const { return ObjectClass; }

    // 초기화 정보 설정자
    FObjectInitializer& SetOuter(UObject* InOuter);
    FObjectInitializer& SetName(const FString& InName);
    FObjectInitializer& SetClass(UClass* InClass);

    // 컴포넌트 생성 헬퍼
    template<typename T>
    T* CreateDefaultSubobject(const FString& SubobjectName) const;

    // UObject 생성 및 초기화
    UObject* CreateObject() const;

    // 유효성 검사
    bool IsValid() const;

private:
    UObject* Outer;          // 소유자 객체
    FString ObjectName;      // 객체 이름
    UClass* ObjectClass;     // 객체 클래스 타입

    // 내부 헬퍼 함수들
    void ValidateInitializer() const;
};

// NewObject 전역 함수들
template<typename T>
T* NewObject(UObject* Outer = nullptr, const FString& Name = FString());

template<typename T>
T* NewObject(UObject* Outer, UClass* Class, const FString& Name = FString());

// 특화된 생성 함수들
template<typename T>
T* NewObject(const FObjectInitializer& ObjectInitializer);

// 템플릿 구현
template<typename T>
T* FObjectInitializer::CreateDefaultSubobject(const FString& SubobjectName) const
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    FObjectInitializer SubInitializer;
    SubInitializer.SetOuter(const_cast<UObject*>(GetOuter()))
                  .SetName(SubobjectName)
                  .SetClass(T::GetStaticClass());

    T* NewSubobject = static_cast<T*>(SubInitializer.CreateObject());
    return NewSubobject;
}

template<typename T>
T* NewObject(UObject* Outer, const FString& Name)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    FObjectInitializer ObjectInitializer(Outer, Name, T::GetStaticClass());
    UObject* NewObj = ObjectInitializer.CreateObject();
    return static_cast<T*>(NewObj);
}

template<typename T>
T* NewObject(UObject* Outer, UClass* Class, const FString& Name)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    FObjectInitializer ObjectInitializer(Outer, Name, Class);
    UObject* NewObj = ObjectInitializer.CreateObject();
    return static_cast<T*>(NewObj);
}

template<typename T>
T* NewObject(const FObjectInitializer& ObjectInitializer)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    UObject* NewObj = ObjectInitializer.CreateObject();
    return static_cast<T*>(NewObj);
}