#include "pch.h"
#include "StaticMeshActor.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "MaterialInterface.h"
#include "ObjectInitializer.h"
#include "KismetProceduralMeshLibrary.h"

IMPLEMENT_CLASS(AStaticMeshActor, AActor)

AStaticMeshActor::AStaticMeshActor()
    : StaticMeshComponent(nullptr)
{
    InitializeActor();
}

AStaticMeshActor::~AStaticMeshActor()
{
}

void AStaticMeshActor::BeginPlay()
{
    Super::BeginPlay();

    // 컴포넌트 유효성 검사
    if (!IsStaticMeshComponentValid())
    {
        // 로그 출력 또는 기본 컴포넌트 생성
        CreateStaticMeshComponent();
    }
}

void AStaticMeshActor::EndPlay()
{
    Super::EndPlay();
}

void AStaticMeshActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AStaticMeshActor::SetStaticMesh(UStaticMesh* InStaticMesh)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetStaticMesh(InStaticMesh);
    }
}

UStaticMesh* AStaticMeshActor::GetStaticMesh() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetStaticMesh();
    }
    return nullptr;
}

void AStaticMeshActor::SetMaterial(int32 MaterialIndex, UMaterialInterface* Material)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetMaterial(MaterialIndex, Material);
    }
}

UMaterialInterface* AStaticMeshActor::GetMaterial(int32 MaterialIndex) const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetMaterial(MaterialIndex);
    }
    return nullptr;
}

int32 AStaticMeshActor::GetNumMaterials() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetNumMaterials();
    }
    return 0;
}

void AStaticMeshActor::SetVisibility(bool bNewVisibility)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetVisibility(bNewVisibility);
    }
}

bool AStaticMeshActor::IsVisible() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->IsVisible();
    }
    return false;
}

void AStaticMeshActor::SetHidden(bool bNewHidden)
{
    if (StaticMeshComponent)
    {
        StaticMeshComponent->SetHidden(bNewHidden);
    }
}

bool AStaticMeshActor::IsHidden() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->IsHidden();
    }
    return true;
}

FVector AStaticMeshActor::GetBoundingBoxMin() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBoundingBoxMin();
    }
    return FVector::Zero;
}

FVector AStaticMeshActor::GetBoundingBoxMax() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBoundingBoxMax();
    }
    return FVector::Zero;
}

FVector AStaticMeshActor::GetBoundingBoxCenter() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBoundingBoxCenter();
    }
    return FVector::Zero;
}

FVector AStaticMeshActor::GetBoundingBoxExtent() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBoundingBoxExtent();
    }
    return FVector::Zero;
}

float AStaticMeshActor::GetBoundingSphereRadius() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBoundingSphereRadius();
    }
    return 0.0f;
}

FBoxSphereBounds AStaticMeshActor::GetBounds() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetBounds();
    }
    return FBoxSphereBounds();
}

uint32 AStaticMeshActor::GetNumVertices() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetNumVertices();
    }
    return 0;
}

uint32 AStaticMeshActor::GetNumTriangles() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetNumTriangles();
    }
    return 0;
}

int32 AStaticMeshActor::GetNumSections() const
{
    if (StaticMeshComponent)
    {
        return StaticMeshComponent->GetNumSections();
    }
    return 0;
}

// 정적 팩토리 함수들
AStaticMeshActor* AStaticMeshActor::CreateWithMesh(UStaticMesh* InStaticMesh, const FVector& Location)
{
    AStaticMeshActor* NewActor = NewObject<AStaticMeshActor>(nullptr, FName("StaticMeshActor"));
    if (NewActor)
    {
        NewActor->SetActorLocation(Location);
        NewActor->SetStaticMesh(InStaticMesh);
    }
    return NewActor;
}

AStaticMeshActor* AStaticMeshActor::CreateWithCubeMesh(const FVector& Location, const FVector& BoxRadius)
{
    FStaticMeshRenderData CubeMeshData = UKismetProceduralMeshLibrary::CreateCubeMesh(BoxRadius);
    UStaticMesh* CubeMesh = NewObject<UStaticMesh>(nullptr, FName("CubeMesh"));
    CubeMesh->SetRenderData(CubeMeshData);
    CubeMesh->BuildDefaultMaterialsAndSections();
    return CreateWithMesh(CubeMesh, Location);
}

AStaticMeshActor* AStaticMeshActor::CreateWithSphereMesh(const FVector& Location, float SphereRadius, int32 SphereSegments, int32 SphereRings)
{
    FStaticMeshRenderData SphereMeshData = UKismetProceduralMeshLibrary::CreateSphereMesh(SphereRadius, SphereSegments, SphereRings);
    UStaticMesh* SphereMesh = NewObject<UStaticMesh>(nullptr, FName("SphereMesh"));
    SphereMesh->SetRenderData(SphereMeshData);
    SphereMesh->BuildDefaultMaterialsAndSections();
    return CreateWithMesh(SphereMesh, Location);
}

AStaticMeshActor* AStaticMeshActor::CreateWithPlaneMesh(const FVector& Location, const FVector& PlaneSize, int32 WidthSegments, int32 HeightSegments)
{
    FStaticMeshRenderData PlaneMeshData = UKismetProceduralMeshLibrary::CreatePlaneMesh(PlaneSize, WidthSegments, HeightSegments);
    UStaticMesh* PlaneMesh = NewObject<UStaticMesh>(nullptr, FName("PlaneMesh"));
    PlaneMesh->SetRenderData(PlaneMeshData);
    PlaneMesh->BuildDefaultMaterialsAndSections();
    return CreateWithMesh(PlaneMesh, Location);
}

AStaticMeshActor* AStaticMeshActor::CreateWithCylinderMesh(const FVector& Location, float CylinderRadius, float CylinderHeight, int32 CylinderSegments)
{
    FStaticMeshRenderData CylinderMeshData = UKismetProceduralMeshLibrary::CreateCylinderMesh(CylinderRadius, CylinderHeight, CylinderSegments);
    UStaticMesh* CylinderMesh = NewObject<UStaticMesh>(nullptr, FName("CylinderMesh"));
    CylinderMesh->SetRenderData(CylinderMeshData);
    CylinderMesh->BuildDefaultMaterialsAndSections();
    return CreateWithMesh(CylinderMesh, Location);
}

AStaticMeshActor* AStaticMeshActor::CreateWithConeMesh(const FVector& Location, float ConeRadius, float ConeHeight, int32 ConeSegments)
{
    FStaticMeshRenderData ConeMeshData = UKismetProceduralMeshLibrary::CreateConeMesh(ConeRadius, ConeHeight, ConeSegments);
    UStaticMesh* ConeMesh = NewObject<UStaticMesh>(nullptr, FName("ConeMesh"));
    ConeMesh->SetRenderData(ConeMeshData);
    ConeMesh->BuildDefaultMaterialsAndSections();
    return CreateWithMesh(ConeMesh, Location);
}

// Protected 함수들
void AStaticMeshActor::InitializeActor()
{
    // 기본 설정
    SetCanEverTick(false); // 정적 메시 액터는 기본적으로 틱 비활성화

    // 컴포넌트 설정
    SetupComponents();
}

void AStaticMeshActor::SetupComponents()
{
    CreateStaticMeshComponent();
}

bool AStaticMeshActor::IsStaticMeshComponentValid() const
{
    return StaticMeshComponent != nullptr;
}

// Private 함수들
void AStaticMeshActor::CreateStaticMeshComponent()
{
    if (!StaticMeshComponent)
    {
        StaticMeshComponent = CreateComponent<UStaticMeshComponent>(FName("StaticMeshComponent"));

        // 루트 컴포넌트로 설정
        SetRootComponent(StaticMeshComponent);
    }
}