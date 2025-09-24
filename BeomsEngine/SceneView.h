#pragma once
#include "Types.h"
#include "Math.h"
#include "Vector.h"
#include "Matrix.h"

enum class EProjectionType
{
    Perspective,
    Orthographic
};

struct FSceneViewInitOptions
{
    FVector ViewLocation = FVector::Zero;
    FVector ViewRotation = FVector::Zero;
    EProjectionType ProjectionType = EProjectionType::Perspective;
    float FOV = 90.0f;
    float AspectRatio = 16.0f / 9.0f;
    float NearPlane = 1.0f;
    float FarPlane = 10000.0f;
    float OrthoWidth = 1024.0f;
    float OrthoHeight = 768.0f;
    uint32 ViewportWidth = 1920;
    uint32 ViewportHeight = 1080;
};

struct FSceneView
{
public:
    FVector ViewLocation;
    FVector ViewRotation;
    FMatrix ViewMatrix;
    FMatrix ProjectionMatrix;
    FMatrix ViewProjectionMatrix;

    EProjectionType ProjectionType;
    float FOV;
    float AspectRatio;
    float NearPlane;
    float FarPlane;
    float OrthoWidth;
    float OrthoHeight;

    uint32 ViewportWidth;
    uint32 ViewportHeight;

private:
    bool bViewMatrixDirty = true;
    bool bProjectionMatrixDirty = true;

public:
    FSceneView() = default;
    FSceneView(const FSceneViewInitOptions& InitOptions);

    void SetViewLocation(const FVector& NewLocation);
    void SetViewRotation(const FVector& NewRotation);
    void SetProjectionType(EProjectionType NewType);
    void SetFOV(float NewFOV);
    void SetAspectRatio(float NewAspectRatio);
    void SetNearFarPlanes(float NewNear, float NewFar);
    void SetOrthoSize(float Width, float Height);
    void SetViewportSize(uint32 Width, uint32 Height);

    const FMatrix& GetViewMatrix();
    const FMatrix& GetProjectionMatrix();
    const FMatrix& GetViewProjectionMatrix();

    FVector GetViewDirection() const;
    FVector GetRightVector() const;
    FVector GetUpVector() const;

    void UpdateMatrices();

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
};