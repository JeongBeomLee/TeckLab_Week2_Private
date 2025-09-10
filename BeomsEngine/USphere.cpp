#include "pch.h"
#include "USphere.h"
#include "ObjectMacros.h"

// RTTI 매크로 구현
IMPLEMENT_CLASS(USphere, UObject)

USphere::USphere()
    : Super()
    , Radius(1.0f)
{
    SetName(TEXT("USphere"));
}

USphere::~USphere()
{
}