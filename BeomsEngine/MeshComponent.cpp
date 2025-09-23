#include "pch.h"
#include "MeshComponent.h"
#include "MaterialInterface.h"

IMPLEMENT_CLASS(UMeshComponent, UPrimitiveComponent)

UMeshComponent::UMeshComponent()
    : bWireframeMode(false)
    , CachedBoundingBoxMin(FVector::Zero)
    , CachedBoundingBoxMax(FVector::Zero)
    , CachedBoundingSphereRadius(0.0f)
    , bBoundingCacheValid(false)
{
}

UMeshComponent::~UMeshComponent()
{
}

void UMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    // 바운딩 정보 업데이트
    InvalidateBounds();
}

void UMeshComponent::EndPlay()
{
    Super::EndPlay();
}

void UMeshComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void UMeshComponent::SetMaterial(int32 MaterialIndex, UMaterialInterface* Material)
{
    SetMaterialOverride(MaterialIndex, Material);
}

UMaterialInterface* UMeshComponent::GetMaterial(int32 MaterialIndex) const
{
    return GetMaterialOverride(MaterialIndex);
}

int32 UMeshComponent::GetNumMaterials() const
{
    return static_cast<int32>(MaterialOverrides.size());
}

void UMeshComponent::SetMaterialOverride(int32 MaterialIndex, UMaterialInterface* Material)
{
    // 배열 크기 확장
    if (MaterialIndex >= MaterialOverrides.size())
    {
        MaterialOverrides.resize(MaterialIndex + 1, nullptr);
    }

    MaterialOverrides[MaterialIndex] = Material;
    MarkRenderStateDirty();
}

UMaterialInterface* UMeshComponent::GetMaterialOverride(int32 MaterialIndex) const
{
    if (MaterialIndex >= 0 && MaterialIndex < MaterialOverrides.size())
    {
        return MaterialOverrides[MaterialIndex];
    }
    return nullptr;
}

void UMeshComponent::ClearMaterialOverrides()
{
    MaterialOverrides.clear();
    MarkRenderStateDirty();
}

FVector UMeshComponent::GetBoundingBoxMin() const
{
    if (!bBoundingCacheValid)
    {
        UpdateBounds();
    }
    return CachedBoundingBoxMin;
}

FVector UMeshComponent::GetBoundingBoxMax() const
{
    if (!bBoundingCacheValid)
    {
        UpdateBounds();
    }
    return CachedBoundingBoxMax;
}

float UMeshComponent::GetBoundingSphereRadius() const
{
    if (!bBoundingCacheValid)
    {
        UpdateBounds();
    }
    return CachedBoundingSphereRadius;
}

FBoxSphereBounds UMeshComponent::GetBounds() const
{
    if (!bBoundingCacheValid)
    {
        UpdateBounds();
    }

    FVector Origin = (CachedBoundingBoxMin + CachedBoundingBoxMax) * 0.5f;
    FVector BoxExtent = (CachedBoundingBoxMax - CachedBoundingBoxMin) * 0.5f;

    return FBoxSphereBounds(Origin, BoxExtent, CachedBoundingSphereRadius);
}

bool UMeshComponent::ShouldRender() const
{
    return Super::ShouldRender() && HasValidMeshData();
}

void UMeshComponent::MarkRenderStateDirty()
{
    Super::MarkRenderStateDirty();

    // 추가적인 메시 컴포넌트 렌더링 상태 마킹
}

void UMeshComponent::UpdateRenderState()
{
    Super::UpdateRenderState();

    // 메시 컴포넌트 특화 렌더링 상태 업데이트
}

void UMeshComponent::InvalidateBounds()
{
    bBoundingCacheValid = false;
}

void UMeshComponent::UpdateBounds() const
{
    // 기본 구현: 원점 중심의 작은 박스
    CachedBoundingBoxMin = FVector(-1.0f, -1.0f, -1.0f);
    CachedBoundingBoxMax = FVector(1.0f, 1.0f, 1.0f);
    CachedBoundingSphereRadius = 1.732f; // sqrt(3)

    bBoundingCacheValid = true;

    // 하위 클래스에서 실제 메시 데이터를 기반으로 바운딩 계산
}

UMaterialInterface* UMeshComponent::GetEffectiveMaterial(int32 MaterialIndex) const
{
    // 오버라이드가 있으면 오버라이드 사용, 없으면 기본 머티리얼 사용
    UMaterialInterface* Override = GetMaterialOverride(MaterialIndex);
    if (Override)
    {
        return Override;
    }

    // 하위 클래스에서 기본 머티리얼 제공
    return nullptr;
}