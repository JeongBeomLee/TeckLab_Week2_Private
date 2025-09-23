#pragma once
#include "SceneComponent.h"
#include "BoxSphereBounds.h"

// 렌더링과 물리적 상호작용이 가능한 컴포넌트의 기본 클래스
class UPrimitiveComponent : public USceneComponent
{
    UCLASS()
    GENERATED_BODY(UPrimitiveComponent, USceneComponent)

public:
    UPrimitiveComponent();
    virtual ~UPrimitiveComponent();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "UPrimitiveComponent"; }

    // USceneComponent 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 렌더링 관련
    virtual bool ShouldRender() const { return bVisible && !bHidden; }
    virtual void SetVisibility(bool bNewVisibility);
    bool IsVisible() const { return bVisible; }

    virtual void SetHidden(bool bNewHidden);
    bool IsHidden() const { return bHidden; }

    // 머티리얼 관리
    virtual void SetMaterial(int32 MaterialIndex, class UMaterialInterface* Material) {}
    virtual class UMaterialInterface* GetMaterial(int32 MaterialIndex) const { return nullptr; }
    virtual int32 GetNumMaterials() const { return 0; }

    // 바운딩 박스/스피어
    virtual FVector GetBoundingBoxMin() const { return FVector::Zero; }
    virtual FVector GetBoundingBoxMax() const { return FVector::Zero; }
    virtual FVector GetBoundingBoxCenter() const;
    virtual FVector GetBoundingBoxExtent() const;
    virtual float GetBoundingSphereRadius() const { return 0.0f; }

    // 통합 바운딩 정보
    virtual FBoxSphereBounds GetBounds() const;

    // 레이캐스팅/트레이싱
    virtual bool LineTraceComponent(const FVector& Start, const FVector& End, FVector& OutHitLocation) const;

protected:
    // 렌더링 플래그들
    bool bVisible;
    bool bHidden;

    // 렌더링 상태 업데이트
    virtual void MarkRenderStateDirty() {}
    virtual void UpdateRenderState() {}
};