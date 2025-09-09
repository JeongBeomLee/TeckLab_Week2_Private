#include "pch.h"
#include "Object.h"

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