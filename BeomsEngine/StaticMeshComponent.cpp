#include "pch.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "MaterialInterface.h"

IMPLEMENT_CLASS(UStaticMeshComponent, UMeshComponent)

UStaticMeshComponent::UStaticMeshComponent()
    : StaticMesh(nullptr)
{
}

UStaticMeshComponent::~UStaticMeshComponent()
{
}

void UStaticMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    // 정적 메시 유효성 검사
    if (!ValidateStaticMesh())
    {
        // 로그 출력 또는 기본 메시 설정
    }
}

void UStaticMeshComponent::EndPlay()
{
    Super::EndPlay();
}

void UStaticMeshComponent::SetStaticMesh(UStaticMesh* InStaticMesh)
{
    if (StaticMesh != InStaticMesh)
    {
        StaticMesh = InStaticMesh;
        OnStaticMeshChanged();
    }
}

void UStaticMeshComponent::SetMaterial(int32 MaterialIndex, UMaterialInterface* Material)
{
    // 먼저 오버라이드 설정
    Super::SetMaterial(MaterialIndex, Material);

    // 정적 메시에도 설정 (선택적)
    if (StaticMesh && MaterialIndex >= 0)
    {
        StaticMesh->SetMaterial(MaterialIndex, Material);
    }
}

UMaterialInterface* UStaticMeshComponent::GetMaterial(int32 MaterialIndex) const
{
    return GetEffectiveMaterial(MaterialIndex);
}

int32 UStaticMeshComponent::GetNumMaterials() const
{
    if (StaticMesh)
    {
        return StaticMesh->GetNumMaterials();
    }
    return Super::GetNumMaterials();
}

uint32 UStaticMeshComponent::GetNumVertices() const
{
    if (StaticMesh)
    {
		return StaticMesh->GetRenderData().NumVertices;
    }
    return 0;
}

uint32 UStaticMeshComponent::GetNumTriangles() const
{
    if (StaticMesh)
    {
        return StaticMesh->GetRenderData().NumTriangles;
    }
    return 0;
}

bool UStaticMeshComponent::HasValidMeshData() const
{
    return StaticMesh && StaticMesh->HasValidRenderData();
}

FVector UStaticMeshComponent::GetBoundingBoxMin() const
{
    if (!HasValidMeshData())
    {
        return FVector::Zero;
    }

    return StaticMesh->GetRenderData().GetBoundingBoxMin();
}

FVector UStaticMeshComponent::GetBoundingBoxMax() const
{
    if (!HasValidMeshData())
    {
        return FVector::Zero;
    }

    return StaticMesh->GetRenderData().GetBoundingBoxMax();
}

float UStaticMeshComponent::GetBoundingSphereRadius() const
{
    if (!HasValidMeshData())
    {
        return 0.0f;
    }

    return StaticMesh->GetRenderData().GetBoundingSphereRadius();
}

FBoxSphereBounds UStaticMeshComponent::GetBounds() const
{
    if (!HasValidMeshData())
    {
        return FBoxSphereBounds();
    }

    return StaticMesh->GetBounds();
}

int32 UStaticMeshComponent::GetNumSections() const
{
    if (StaticMesh)
    {
        return StaticMesh->GetNumSections();
    }
    return 0;
}

const FStaticMeshSection* UStaticMeshComponent::GetSection(int32 SectionIndex) const
{
    if (StaticMesh && SectionIndex >= 0 && SectionIndex < StaticMesh->GetNumSections())
    {
        const TArray<FStaticMeshSection>& Sections = StaticMesh->GetSections();
        return &Sections[SectionIndex];
    }
    return nullptr;
}

const FStaticMeshRenderData* UStaticMeshComponent::GetRenderData() const
{
    if (StaticMesh)
    {
        return &StaticMesh->GetRenderData();
    }
    return nullptr;
}

void UStaticMeshComponent::SetInstanceData(const TArray<FMatrix>& InstanceTransforms)
{
    this->InstanceTransforms = InstanceTransforms;
    MarkRenderStateDirty();
}

void UStaticMeshComponent::UpdateBounds() const
{
    if (!HasValidMeshData())
    {
        // 기본 바운딩 사용
        Super::UpdateBounds();
        return;
    }

    // FBoxSphereBounds로부터 캐시된 바운딩 정보 복사
    const FBoxSphereBounds& MeshBounds = StaticMesh->GetRenderData().Bounds;

    CachedBoundingBoxMin = MeshBounds.GetBoxMin();
    CachedBoundingBoxMax = MeshBounds.GetBoxMax();
    CachedBoundingSphereRadius = MeshBounds.SphereRadius;

    bBoundingCacheValid = true;
}

void UStaticMeshComponent::MarkRenderStateDirty()
{
    Super::MarkRenderStateDirty();

    // 정적 메시 컴포넌트 특화 렌더링 상태 마킹
}

void UStaticMeshComponent::UpdateRenderState()
{
    Super::UpdateRenderState();

    // 정적 메시 컴포넌트 특화 렌더링 상태 업데이트
}

void UStaticMeshComponent::OnStaticMeshChanged()
{
    // 바운딩 캐시 무효화
    InvalidateBounds();

    // 머티리얼 오버라이드 조정
    if (StaticMesh)
    {
        int32 NumMaterials = StaticMesh->GetNumMaterials();
        if (MaterialOverrides.size() > NumMaterials)
        {
            MaterialOverrides.resize(NumMaterials);
        }
    }

    // 렌더링 상태 갱신
    MarkRenderStateDirty();
}

UMaterialInterface* UStaticMeshComponent::GetEffectiveMaterial(int32 MaterialIndex) const
{
    // 먼저 오버라이드 확인
    UMaterialInterface* Override = Super::GetEffectiveMaterial(MaterialIndex);
    if (Override)
    {
        return Override;
    }

    // 정적 메시의 기본 머티리얼 사용
    if (StaticMesh)
    {
        return StaticMesh->GetMaterial(MaterialIndex);
    }

    return nullptr;
}

bool UStaticMeshComponent::ValidateStaticMesh() const
{
    return StaticMesh && StaticMesh->HasValidRenderData();
}