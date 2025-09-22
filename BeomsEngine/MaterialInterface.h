#pragma once

enum class EBlendMode : uint8
{
    BLEND_Opaque,
    BLEND_Masked,
    BLEND_Translucent,
    BLEND_Additive,
};

enum class EShadingModel : uint8
{
    MSM_DefaultLit,
    MSM_Unlit,
};

struct FMaterialParameterInfo
{
    FName Name;
    int32 Index;

    FMaterialParameterInfo()
        : Index(0)
    {}

    FMaterialParameterInfo(const FName& InName, int32 InIndex = 0)
        : Name(InName)
        , Index(InIndex)
    {}
};

class UMaterialInterface
{
public:
    UMaterialInterface()
        : BlendMode(EBlendMode::BLEND_Opaque)
        , ShadingModel(EShadingModel::MSM_DefaultLit)
        , Opacity(1.0f)
        , OpacityMaskClipValue(0.3333f)
        , bIsTwoSided(false)
        , bUsedWithStaticLighting(true)
        , bUsedWithInstancedStaticMeshes(false)
        , BaseColor(FVector(0.18f, 0.18f, 0.18f))
        , Metallic(0.0f)
        , Specular(0.5f)
        , Roughness(0.5f)
        , EmissiveColor(FVector::Zero)
        , Normal(FVector(0.0f, 0.0f, 1.0f))
        , WorldPositionOffset(FVector::Zero)
    {}

    virtual ~UMaterialInterface() = default;

    // 블렌드 모드 및 셰이딩 모델
    EBlendMode BlendMode;
    EShadingModel ShadingModel;

    // 기본 머티리얼 속성
    float Opacity;
    float OpacityMaskClipValue;

    // 머티리얼 플래그
    bool bIsTwoSided;
    bool bUsedWithStaticLighting;
    bool bUsedWithInstancedStaticMeshes;

    // PBR 머티리얼 입력
    FVector BaseColor;          // 기본 색상 (Albedo)
    float Metallic;             // 금속성 (0=비금속, 1=금속)
    float Specular;             // 반사율 (일반적으로 0.5)
    float Roughness;            // 거칠기 (0=매끄러움, 1=거침)
    FVector EmissiveColor;      // 발광 색상
    FVector Normal;             // 법선 맵
    FVector WorldPositionOffset; // 월드 위치 오프셋

    // 텍스처 경로들
    FString DiffuseTexturePath;
    FString NormalTexturePath;
    FString SpecularTexturePath;
    FString RoughnessTexturePath;
    FString MetallicTexturePath;
    FString EmissiveTexturePath;
    FString OpacityTexturePath;

    // 머티리얼 이름
    FName MaterialName;

    // 가상 함수들
    virtual bool IsUsedWithStaticLighting() const { return bUsedWithStaticLighting; }
    virtual bool IsUsedWithInstancedStaticMeshes() const { return bUsedWithInstancedStaticMeshes; }
    virtual bool IsTwoSided() const { return bIsTwoSided; }
    virtual bool IsMasked() const { return BlendMode == EBlendMode::BLEND_Masked; }
    virtual bool IsTranslucent() const { return BlendMode == EBlendMode::BLEND_Translucent || BlendMode == EBlendMode::BLEND_Additive; }

    // 파라미터 접근 함수들
    virtual bool GetScalarParameterValue(const FMaterialParameterInfo& ParameterInfo, float& OutValue) const
    {
        if (ParameterInfo.Name == FName("Opacity"))
        {
            OutValue = Opacity;
            return true;
        }
        else if (ParameterInfo.Name == FName("Metallic"))
        {
            OutValue = Metallic;
            return true;
        }
        else if (ParameterInfo.Name == FName("Specular"))
        {
            OutValue = Specular;
            return true;
        }
        else if (ParameterInfo.Name == FName("Roughness"))
        {
            OutValue = Roughness;
            return true;
        }
        return false;
    }

    virtual bool GetVectorParameterValue(const FMaterialParameterInfo& ParameterInfo, FVector& OutValue) const
    {
        if (ParameterInfo.Name == FName("BaseColor"))
        {
            OutValue = BaseColor;
            return true;
        }
        else if (ParameterInfo.Name == FName("EmissiveColor"))
        {
            OutValue = EmissiveColor;
            return true;
        }
        else if (ParameterInfo.Name == FName("Normal"))
        {
            OutValue = Normal;
            return true;
        }
        return false;
    }

    // 머티리얼 유효성 검사
    virtual bool IsValidLowLevel() const { return true; }

    // 머티리얼 리소스 관련
    virtual void BeginDestroy() {}
    virtual bool IsReadyForFinishDestroy() { return true; }
    virtual void FinishDestroy() {}

    // 셰이더 컴파일 관련
    virtual bool IsCompilationFinished() const { return true; }
    virtual void CacheShaders() {}

    // 디버그 정보
    virtual FString GetDesc() const
    {
        return MaterialName.IsNone() ? "UMaterialInterface" : MaterialName.ToString();
    }

    // 팩토리 함수들
    static UMaterialInterface* CreateDefaultMaterial(const FName& Name = FName("DefaultMaterial"))
    {
        UMaterialInterface* Material = new UMaterialInterface();
        Material->MaterialName = Name;
        Material->BaseColor = FVector(0.18f, 0.18f, 0.18f); // 기본 회색
        Material->Metallic = 0.0f;
        Material->Specular = 0.5f;
        Material->Roughness = 0.5f;
        return Material;
    }

    static UMaterialInterface* CreatePBRMaterial(
        const FName& Name,
        const FVector& InBaseColor,
        float InMetallic,
        float InRoughness,
        float InSpecular = 0.5f)
    {
        UMaterialInterface* Material = new UMaterialInterface();
        Material->MaterialName = Name;
        Material->BaseColor = InBaseColor;
        Material->Metallic = InMetallic;
        Material->Roughness = InRoughness;
        Material->Specular = InSpecular;
        return Material;
    }
};

// 머티리얼 인스턴스 (동적으로 파라미터를 변경할 수 있는 머티리얼)
class UMaterialInstanceInterface : public UMaterialInterface
{
public:
    UMaterialInstanceInterface(UMaterialInterface* InParent = nullptr)
        : Parent(InParent)
    {}

    virtual ~UMaterialInstanceInterface() = default;

    // 부모 머티리얼
    UMaterialInterface* Parent;

    // 오버라이드된 파라미터들
    TArray<TPair<FName, float>> ScalarParameterOverrides;
    TArray<TPair<FName, FVector>> VectorParameterOverrides;

    // 파라미터 설정
    void SetScalarParameterValue(const FName& ParameterName, float Value)
    {
        for (auto& Override : ScalarParameterOverrides)
        {
            if (Override.first == ParameterName)
            {
                Override.second = Value;
                return;
            }
        }
        ScalarParameterOverrides.push_back(std::make_pair(ParameterName, Value));
    }

    void SetVectorParameterValue(const FName& ParameterName, const FVector& Value)
    {
        for (auto& Override : VectorParameterOverrides)
        {
            if (Override.first == ParameterName)
            {
                Override.second = Value;
                return;
            }
        }
        VectorParameterOverrides.push_back(std::make_pair(ParameterName, Value));
    }

    // 파라미터 값 가져오기 (오버라이드 고려)
    virtual bool GetScalarParameterValue(const FMaterialParameterInfo& ParameterInfo, float& OutValue) const override
    {
        // 먼저 오버라이드에서 찾기
        for (const auto& Override : ScalarParameterOverrides)
        {
            if (Override.first == ParameterInfo.Name)
            {
                OutValue = Override.second;
                return true;
            }
        }

        // 오버라이드에 없으면 부모에서 찾기
        if (Parent)
        {
            return Parent->GetScalarParameterValue(ParameterInfo, OutValue);
        }

        return UMaterialInterface::GetScalarParameterValue(ParameterInfo, OutValue);
    }

    virtual bool GetVectorParameterValue(const FMaterialParameterInfo& ParameterInfo, FVector& OutValue) const override
    {
        // 먼저 오버라이드에서 찾기
        for (const auto& Override : VectorParameterOverrides)
        {
            if (Override.first == ParameterInfo.Name)
            {
                OutValue = Override.second;
                return true;
            }
        }

        // 오버라이드에 없으면 부모에서 찾기
        if (Parent)
        {
            return Parent->GetVectorParameterValue(ParameterInfo, OutValue);
        }

        return UMaterialInterface::GetVectorParameterValue(ParameterInfo, OutValue);
    }
};