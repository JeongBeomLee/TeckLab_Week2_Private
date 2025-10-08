#pragma once
#include "StaticMeshRenderData.h"

// 전방 선언
class UMaterialInterface;
class FStaticMeshRenderResource;

// UStaticMesh - 정적 메시 에셋 클래스
class UStaticMesh : public UObject
{
    UCLASS()
    GENERATED_BODY(UStaticMesh, UObject)

public:
    UStaticMesh();
    virtual ~UStaticMesh();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "UStaticMesh"; }

    // 렌더링 데이터 접근
    const FStaticMeshRenderData& GetRenderData() const { return RenderData; }
    FStaticMeshRenderData& GetRenderData() { return RenderData; }

    // 렌더링 데이터 설정
    void SetRenderData(const FStaticMeshRenderData& InRenderData);
    void SetRenderData(const FString& InFilePath, const TArray<FVertex>& InVertices, const TArray<uint32>& InIndices);

    // 머티리얼 관리
    void AddMaterialSlot(UMaterialInterface* Material);
    void SetMaterial(int32 MaterialIndex, UMaterialInterface* Material);

    UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
    int32 GetNumMaterials() const { return static_cast<int32>(Materials.size()); }

    // GPU 렌더링 리소스 접근
    FStaticMeshRenderResource* GetRenderResource() const { return RenderResource; }

    // GPU 렌더링 리소스 초기화 (SetRenderData 후 호출 필요)
    void InitializeRenderResources(ID3D11Device* Device);

    // 섹션 관리
    void AddSection(uint32 MaterialIndex, uint32 FirstIndex, uint32 NumTriangles, uint32 MinVertexIndex, uint32 MaxVertexIndex);
    void ClearSections() { Sections.clear(); }
    const TArray<FStaticMeshSection>& GetSections() const { return Sections; }
    int32 GetNumSections() const { return static_cast<int32>(Sections.size()); }

    // 메시 정보
    bool HasValidRenderData() const;

    // 바운딩 정보 접근
    FVector GetBoundingBoxMin() const;
    FVector GetBoundingBoxMax() const;
    FVector GetBoundingBoxCenter() const;
    FVector GetBoundingBoxExtent() const;
    float GetBoundingSphereRadius() const;
    const FBoxSphereBounds& GetBounds() const;

    // 메시 데이터 빌드 (OBJ 등에서 로드 후 호출)
    void BuildFromObjData(const FObjInfo& ObjData);
    void BuildDefaultMaterialsAndSections();

private:
    // 렌더링 데이터 (CPU: 순수 정점/인덱스 데이터)
    FStaticMeshRenderData RenderData;

    // GPU 렌더링 리소스
    FStaticMeshRenderResource* RenderResource;

    // 머티리얼 슬롯들
    TArray<UMaterialInterface*> Materials;

    // 렌더링 섹션들 (머티리얼별 그리기 단위)
    TArray<FStaticMeshSection> Sections;
};
