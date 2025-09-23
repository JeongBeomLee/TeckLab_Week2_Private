#pragma once
#include "Actor.h"
#include "BoxSphereBounds.h"

// 전방 선언
class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

// 정적 메시를 렌더링하는 액터 클래스
class AStaticMeshActor : public AActor
{
    UCLASS()
    GENERATED_BODY(AStaticMeshActor, AActor)

public:
    AStaticMeshActor();
    virtual ~AStaticMeshActor();

    // UObject 오버라이드
    virtual FString GetClassName() const override { return "AStaticMeshActor"; }

    // AActor 오버라이드
    virtual void BeginPlay() override;
    virtual void EndPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 정적 메시 컴포넌트 접근
    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

    // 정적 메시 관리 (편의 함수들)
    void SetStaticMesh(UStaticMesh* InStaticMesh);
    UStaticMesh* GetStaticMesh() const;

    // 머티리얼 관리 (StaticMeshComponent로 위임)
    void SetMaterial(int32 MaterialIndex, class UMaterialInterface* Material);
    class UMaterialInterface* GetMaterial(int32 MaterialIndex) const;
    int32 GetNumMaterials() const;

    // 렌더링 설정
    void SetVisibility(bool bNewVisibility);
    bool IsVisible() const;

    void SetHidden(bool bNewHidden);
    bool IsHidden() const;

    // 바운딩 정보 (StaticMeshComponent로 위임)
    FVector GetBoundingBoxMin() const;
    FVector GetBoundingBoxMax() const;
    FVector GetBoundingBoxCenter() const;
    FVector GetBoundingBoxExtent() const;
    float GetBoundingSphereRadius() const;
    FBoxSphereBounds GetBounds() const;

    // 메시 정보
    uint32 GetNumVertices() const;
    uint32 GetNumTriangles() const;
    int32 GetNumSections() const;

    // 정적 팩토리 함수들
    static AStaticMeshActor* CreateWithMesh(UStaticMesh* InStaticMesh, const FVector& Location = FVector::Zero);
    static AStaticMeshActor* CreateWithCubeMesh(const FVector& Location = FVector::Zero, const FVector& BoxRadius = FVector(50.0f, 50.0f, 50.0f));
    static AStaticMeshActor* CreateWithSphereMesh(const FVector& Location = FVector::Zero, float SphereRadius = 50.0f, int32 SphereSegments = 32, int32 SphereRings = 16);
    static AStaticMeshActor* CreateWithPlaneMesh(const FVector& Location = FVector::Zero, const FVector& PlaneSize = FVector(100.0f, 100.0f, 0.0f), int32 WidthSegments = 1, int32 HeightSegments = 1);
    static AStaticMeshActor* CreateWithCylinderMesh(const FVector& Location = FVector::Zero, float CylinderRadius = 50.0f, float CylinderHeight = 100.0f, int32 CylinderSegments = 32);
    static AStaticMeshActor* CreateWithConeMesh(const FVector& Location = FVector::Zero, float ConeRadius = 50.0f, float ConeHeight = 100.0f, int32 ConeSegments = 32);

protected:
    // 메인 컴포넌트
    UStaticMeshComponent* StaticMeshComponent;

    // 액터 초기화
    virtual void InitializeActor();
    virtual void SetupComponents();

    // 컴포넌트 유효성 검사
    bool IsStaticMeshComponentValid() const;

private:
    // 내부 헬퍼 함수들
    void CreateStaticMeshComponent();
};