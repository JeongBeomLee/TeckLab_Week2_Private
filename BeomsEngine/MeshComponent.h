#pragma once
#include "PrimitiveComponent.h"

// 메시 렌더링이 가능한 컴포넌트의 기본 클래스
class UMeshComponent : public UPrimitiveComponent
{
    UCLASS()
    GENERATED_BODY(UMeshComponent, UPrimitiveComponent)

public:
    UMeshComponent();
    virtual ~UMeshComponent();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "UMeshComponent"; }

    // UPrimitiveComponent 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 머티리얼 관리
    virtual void SetMaterial(int32 MaterialIndex, class UMaterialInterface* Material) override;
    virtual class UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
    virtual int32 GetNumMaterials() const override;

    // 머티리얼 오버라이드 (런타임에서 머티리얼 변경)
    void SetMaterialOverride(int32 MaterialIndex, class UMaterialInterface* Material);
    class UMaterialInterface* GetMaterialOverride(int32 MaterialIndex) const;
    void ClearMaterialOverrides();

    // 바운딩 관련
    virtual FVector GetBoundingBoxMin() const override;
    virtual FVector GetBoundingBoxMax() const override;
    virtual float GetBoundingSphereRadius() const override;
    virtual FBoxSphereBounds GetBounds() const override;

    // 메시 데이터 접근 (하위 클래스에서 구현)
    virtual uint32 GetNumVertices() const { return 0; }
    virtual uint32 GetNumTriangles() const { return 0; }
    virtual bool HasValidMeshData() const { return false; }

    // 렌더링 상태
    virtual bool ShouldRender() const override;

    // 커스텀 프로퍼티들
    void SetWireframeMode(bool bWireframe) { bWireframeMode = bWireframe; MarkRenderStateDirty(); }
    bool IsWireframeMode() const { return bWireframeMode; }

protected:
    // 머티리얼 오버라이드 배열 (원본 머티리얼을 런타임에 변경)
    TArray<class UMaterialInterface*> MaterialOverrides;

    // 메시 렌더링 플래그들
    bool bWireframeMode;

    // 바운딩 캐시
    mutable FVector CachedBoundingBoxMin;
    mutable FVector CachedBoundingBoxMax;
    mutable float CachedBoundingSphereRadius;
    mutable bool bBoundingCacheValid;

    // 렌더링 상태 업데이트
    virtual void MarkRenderStateDirty() override;
    virtual void UpdateRenderState() override;

    // 바운딩 캐시 무효화
    virtual void InvalidateBounds();
    virtual void UpdateBounds() const;

    // 머티리얼 유틸리티
    class UMaterialInterface* GetEffectiveMaterial(int32 MaterialIndex) const;
};