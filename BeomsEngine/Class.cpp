#include "pch.h"
#include "Class.h"
#include "Object.h"

// 정적 멤버 정의
TArray<UClass*> UClass::RegisteredClasses;
TMap<FName, UClass*> UClass::ClassMap;

UClass::UClass(const FName& InClassName, UClass* InSuperClass, ClassConstructorType InConstructor)
    : ClassName(InClassName)
    , SuperClass(InSuperClass)
    , Constructor(InConstructor)
    , bChildClassesCached(false)
{
}

UClass::~UClass()
{
}

bool UClass::IsChildOf(const UClass* SomeBase) const
{
    if (!SomeBase)
        return false;
        
    if (this == SomeBase)
        return true;
        
    const UClass* CurrentClass = SuperClass;
    while (CurrentClass)
    {
        if (CurrentClass == SomeBase)
            return true;
        CurrentClass = CurrentClass->SuperClass;
    }
    
    return false;
}

bool UClass::IsChildOf(const FName& BaseClassName) const
{
    UClass* BaseClass = FindClass(BaseClassName);
    return IsChildOf(BaseClass);
}

UObject* UClass::CreateDefaultObject() const
{
    if (Constructor)
    {
        return Constructor();
    }
    return nullptr;
}

void UClass::RegisterClass(UClass* NewClass)
{
    if (NewClass)
    {
        RegisteredClasses.push_back(NewClass);
        ClassMap[NewClass->GetName()] = NewClass;
    }
}

UClass* UClass::FindClass(const FName& ClassName)
{
    auto it = ClassMap.find(ClassName);
    if (it != ClassMap.end())
    {
        return it->second;
    }
    return nullptr;
}

void UClass::GetAllChildClasses(TArray<UClass*>& OutChildClasses) const
{
    if (!bChildClassesCached)
    {
        CacheChildClasses();
    }
    
    OutChildClasses = ChildClasses;
}

bool UClass::HasChildren() const
{
    if (!bChildClassesCached)
    {
        CacheChildClasses();
    }
    
    return !ChildClasses.empty();
}

void UClass::CacheChildClasses() const
{
    ChildClasses.clear();
    
    for (UClass* RegisteredClass : RegisteredClasses)
    {
        if (RegisteredClass && RegisteredClass != this && RegisteredClass->IsChildOf(this))
        {
            ChildClasses.push_back(RegisteredClass);
        }
    }
    
    bChildClassesCached = true;
}