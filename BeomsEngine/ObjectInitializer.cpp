#include "pch.h"
#include "ObjectInitializer.h"
#include "Object.h"
#include "Class.h"

FObjectInitializer::FObjectInitializer(UObject* InOuter, const FString& InName, UClass* InClass)
    : Outer(InOuter)
    , ObjectName(InName)
    , ObjectClass(InClass)
{
}

FObjectInitializer::FObjectInitializer(const FObjectInitializer& Other)
    : Outer(Other.Outer)
    , ObjectName(Other.ObjectName)
    , ObjectClass(Other.ObjectClass)
{
}

FObjectInitializer& FObjectInitializer::operator=(const FObjectInitializer& Other)
{
    if (this != &Other)
    {
        Outer = Other.Outer;
        ObjectName = Other.ObjectName;
        ObjectClass = Other.ObjectClass;
    }
    return *this;
}

FObjectInitializer::~FObjectInitializer()
{
}

FObjectInitializer& FObjectInitializer::SetOuter(UObject* InOuter)
{
    Outer = InOuter;
    return *this;
}

FObjectInitializer& FObjectInitializer::SetName(const FString& InName)
{
    ObjectName = InName;
    return *this;
}

FObjectInitializer& FObjectInitializer::SetClass(UClass* InClass)
{
    ObjectClass = InClass;
    return *this;
}

UObject* FObjectInitializer::CreateObject() const
{
    ValidateInitializer();

    // 클래스가 지정되지 않았으면 UObject 기본 클래스 사용
    UClass* ClassToUse = ObjectClass;
    if (!ClassToUse)
    {
        ClassToUse = UObject::GetStaticClass();
    }

    // 클래스의 생성자를 통해 객체 생성
    UObject* NewObject = ClassToUse->CreateDefaultObject();
    if (!NewObject)
    {
        return nullptr;
    }

    // 기본 속성 설정
    if (!ObjectName.empty())
    {
        NewObject->SetName(ObjectName);
    }

    if (Outer)
    {
        NewObject->SetOuter(Outer);
    }

    return NewObject;
}

bool FObjectInitializer::IsValid() const
{
    return true; // 기본적으로 모든 초기화자는 유효
}

void FObjectInitializer::ValidateInitializer() const
{
    // 추후 유효성 검사 로직 추가 가능
    // 예: 순환 참조 검사, 이름 충돌 검사 등
}