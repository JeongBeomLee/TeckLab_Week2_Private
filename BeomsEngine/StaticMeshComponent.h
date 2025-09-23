#pragma once
#include "MeshComponent.h"

// 전방 선언
class UStaticMesh;
struct FStaticMeshSection;
struct FStaticMeshRenderData;

// 정적 메시를 렌더링하는 컴포넌트
class UStaticMeshComponent : public UMeshComponent
{
    UCLASS()
    GENERATED_BODY(UStaticMeshComponent, UMeshComponent)

public:
    UStaticMeshComponent();
    virtual ~UStaticMeshComponent();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "UStaticMeshComponent"; }

    // UMeshComponent 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override;

    // 정적 메시 관리
    void SetStaticMesh(UStaticMesh* InStaticMesh);
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }

    // UMeshComponent 오버라이드 - 머티리얼 관리
    virtual void SetMaterial(int32 MaterialIndex, class UMaterialInterface* Material) override;
    virtual class UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
    virtual int32 GetNumMaterials() const override;

    // UMeshComponent 오버라이드 - 메시 데이터
    virtual uint32 GetNumVertices() const override;
    virtual uint32 GetNumTriangles() const override;
    virtual bool HasValidMeshData() const override;

    // UPrimitiveComponent 오버라이드 - 바운딩
    virtual FVector GetBoundingBoxMin() const override;
    virtual FVector GetBoundingBoxMax() const override;
    virtual float GetBoundingSphereRadius() const override;

    // 바운딩 정보 통합 접근
    virtual FBoxSphereBounds GetBounds() const override;

    // 정적 메시 섹션 관리
    int32 GetNumSections() const;
    const FStaticMeshSection* GetSection(int32 SectionIndex) const;

    // 렌더링 데이터 접근
    const FStaticMeshRenderData* GetRenderData() const;

    // 인스턴싱 지원 (향후 확장용)
    void SetInstanceData(const TArray<FMatrix>& InstanceTransforms);
    const TArray<FMatrix>& GetInstanceData() const { return InstanceTransforms; }
    int32 GetInstanceCount() const { return static_cast<int32>(InstanceTransforms.size()); }
    bool IsInstanced() const { return !InstanceTransforms.empty(); }

protected:
    // 정적 메시 애셋 참조
    UStaticMesh* StaticMesh;

    // 인스턴싱 데이터
    TArray<FMatrix> InstanceTransforms;

    // UMeshComponent 오버라이드
    virtual void UpdateBounds() const override;
    virtual void MarkRenderStateDirty() override;
    virtual void UpdateRenderState() override;

    // 정적 메시 변경 처리
    virtual void OnStaticMeshChanged();

    // 머티리얼 유틸리티
    virtual class UMaterialInterface* GetEffectiveMaterial(int32 MaterialIndex) const;

private:
    // 메시 검증
    bool ValidateStaticMesh() const;
};