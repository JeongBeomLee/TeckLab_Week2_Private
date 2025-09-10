#include "pch.h"
#include "Object.h"
#include "Class.h"
#include "ObjectMacros.h"

// 정적 멤버 초기화
uint64 UObject::NextUniqueID = 1;

UObject::UObject()
    : bIsValid(true)
    , bPendingKill(false) 
    , bHasBegunPlay(false)
    , ObjectName(TEXT("UObject"))
    , UniqueID(GenerateUniqueID())
    , Outer(nullptr)
{
}

UObject::~UObject()
{
    bIsValid = false;
}

uint64 UObject::GenerateUniqueID()
{
    return NextUniqueID++;
}

UClass* UObject::GetClass() const
{
    return UObject::GetStaticClass();
}

UClass* UObject::GetStaticClass()
{
    static UClass* StaticClass = nullptr;
    if (!StaticClass)
    {
        StaticClass = new UClass(
            TEXT("UObject"), 
            nullptr, 
            &UObject::CreateInstance
        );
        UClass::RegisterClass(StaticClass);
    }
    return StaticClass;
}

bool UObject::IsA(const UClass* SomeClass) const
{
    if (!SomeClass)
        return false;
        
    UClass* MyClass = GetClass();
    while (MyClass)
    {
        if (MyClass == SomeClass)
            return true;
        MyClass = MyClass->GetSuperClass();
    }
    return false;
}

bool UObject::IsA(const FString& ClassName) const
{
    UClass* SomeClass = UClass::FindClass(ClassName);
    return IsA(SomeClass);
}

template<typename T>
bool UObject::IsA() const
{
    return IsA(T::GetStaticClass());
}