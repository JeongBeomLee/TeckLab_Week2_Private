#pragma once
#include "StaticMeshRenderData.h"

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

    // 머티리얼 슬롯 관리
    void AddMaterialSlot(const FName& SlotName, class UMaterialInterface* Material = nullptr);
    void SetMaterial(int32 MaterialIndex, class UMaterialInterface* Material);
    void SetMaterialByName(const FName& SlotName, class UMaterialInterface* Material);

    class UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
    class UMaterialInterface* GetMaterialByName(const FName& SlotName) const;
    int32 GetMaterialIndex(const FName& MaterialSlotName) const;

    const TArray<FStaticMaterial>& GetStaticMaterials() const { return StaticMaterials; }
    int32 GetNumMaterials() const { return static_cast<int32>(StaticMaterials.size()); }

    // 섹션 관리
    void AddSection(uint32 MaterialIndex, uint32 FirstIndex, uint32 NumTriangles, uint32 MinVertexIndex, uint32 MaxVertexIndex);
    void ClearSections() { Sections.clear(); }
    const TArray<FStaticMeshSection>& GetSections() const { return Sections; }
    int32 GetNumSections() const { return static_cast<int32>(Sections.size()); }

    // 메시 정보
    bool HasValidRenderData() const;

    // 메시 데이터 빌드 (OBJ 등에서 로드 후 호출)
    void BuildFromObjData(const FObjInfo& ObjData);
    void BuildDefaultMaterialsAndSections();

private:
    // 렌더링 데이터 (순수 정점/인덱스 데이터)
    FStaticMeshRenderData RenderData;

    // 머티리얼 슬롯들
    TArray<FStaticMaterial> StaticMaterials;

    // 렌더링 섹션들 (머티리얼별 그리기 단위)
    TArray<FStaticMeshSection> Sections;
};

// 편의 함수들
namespace StaticMeshUtils
{
    // OBJ 데이터로부터 UStaticMesh 생성
    UStaticMesh* CreateStaticMeshFromObj(const FObjInfo& ObjData, const FString& MeshName = "");

    // 기본 프리미티브 메시들 생성
    UStaticMesh* CreateCubeMesh();
    UStaticMesh* CreateSphereMesh(int32 Segments = 16);
    UStaticMesh* CreatePlaneMesh(float Width = 100.0f, float Height = 100.0f);
}