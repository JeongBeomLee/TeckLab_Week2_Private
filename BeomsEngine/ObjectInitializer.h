#pragma once
#include "Types.h"
#include "String.h"
#include "Name.h"

// 전방 선언
class UObject;
class UClass;

// FObjectInitializer - UObject 생성 시 초기화 정보를 담는 구조체
class FObjectInitializer
{
public:
    // 생성자
    explicit FObjectInitializer(UObject* InOuter = nullptr, const FName& InName = FName(), UClass* InClass = nullptr);

    // 복사 생성자
    FObjectInitializer(const FObjectInitializer& Other);

    // 대입 연산자
    FObjectInitializer& operator=(const FObjectInitializer& Other);

    // 소멸자
    ~FObjectInitializer();

    // 초기화 정보 접근자
    UObject* GetOuter() const { return Outer; }
    const FName& GetName() const { return ObjectName; }
    UClass* GetClass() const { return ObjectClass; }

    // 초기화 정보 설정자
    FObjectInitializer& SetOuter(UObject* InOuter);
    FObjectInitializer& SetName(const FName& InName);
    FObjectInitializer& SetClass(UClass* InClass);

    // UObject 생성 및 초기화
    UObject* CreateObject() const;

    // 유효성 검사
    bool IsValid() const;

private:
    UObject* Outer;          // 소유자 객체
    FName ObjectName;        // 객체 이름
    UClass* ObjectClass;     // 객체 클래스 타입

    // 내부 헬퍼 함수들
    void ValidateInitializer() const;
};

// NewObject 전역 함수들
template<typename T>
T* NewObject(UObject* Outer = nullptr, const FName& Name = FName());

template<typename T>
T* NewObject(UObject* Outer, UClass* Class, const FName& Name = FName());

// 특화된 생성 함수들
template<typename T>
T* NewObject(const FObjectInitializer& ObjectInitializer);

// 전역 CreateDefaultSubobject 함수들
template<typename T>
T* CreateDefaultSubobject(UObject* Outer, const FName& SubobjectName);

template<typename T>
T* CreateDefaultSubobject(UObject* Outer, const FName& SubobjectName, UClass* ComponentClass);

// 자동 이름 생성 버전
template<typename T>
T* CreateDefaultSubobject(UObject* Outer);

template<typename T>
T* NewObject(UObject* Outer, const FName& Name)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    FObjectInitializer ObjectInitializer(Outer, Name, T::GetStaticClass());
    UObject* NewObj = ObjectInitializer.CreateObject();
    return static_cast<T*>(NewObj);
}

template<typename T>
T* NewObject(UObject* Outer, UClass* Class, const FName& Name)
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

// 전역 CreateDefaultSubobject 구현들
template<typename T>
T* CreateDefaultSubobject(UObject* Outer, const FName& SubobjectName)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    if (!Outer)
    {
        return nullptr;
    }

    // 클래스, 이름, Outer 설정
    FObjectInitializer SubInitializer;
    SubInitializer.SetOuter(Outer)
                  .SetName(SubobjectName)
                  .SetClass(T::GetStaticClass());

    T* NewSubobject = static_cast<T*>(SubInitializer.CreateObject());
    return NewSubobject;
}

template<typename T>
T* CreateDefaultSubobject(UObject* Outer, const FName& SubobjectName, UClass* ComponentClass)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    if (!Outer || !ComponentClass)
    {
        return nullptr;
    }

    FObjectInitializer SubInitializer;
    SubInitializer.SetOuter(Outer)
                  .SetName(SubobjectName)
                  .SetClass(ComponentClass);

    T* NewSubobject = static_cast<T*>(SubInitializer.CreateObject());
    return NewSubobject;
}

template<typename T>
T* CreateDefaultSubobject(UObject* Outer)
{
    static_assert(std::is_base_of<UObject, T>::value, "T must derive from UObject");

    if (!Outer)
    {
        return nullptr;
    }

    // 자동 이름 생성: 클래스명 + 고유 ID
    FName AutoName = FName(FString("Default") + T::GetStaticClass()->GetNameString());

    return CreateDefaultSubobject<T>(Outer, AutoName);
}