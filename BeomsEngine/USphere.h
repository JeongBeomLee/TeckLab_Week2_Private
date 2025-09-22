#pragma once
#include "Object.h"
#include "ObjectMacros.h"


class USphere : public UObject
{
    UCLASS()
    GENERATED_BODY(USphere, UObject)

public:
    USphere();
    virtual ~USphere();

    virtual FString GetClassName() const override { return "USphere"; }

    void SetRadius(float InRadius) { Radius = InRadius; }
    float GetRadius() const { return Radius; }

private:
    float Radius;
};