#include "pch.h"
#include "SceneView.h"
#include <cmath>

FSceneView::FSceneView(const FSceneViewInitOptions& InitOptions)
    : ViewLocation(InitOptions.ViewLocation)
    , ViewRotation(InitOptions.ViewRotation)
    , ProjectionType(InitOptions.ProjectionType)
    , FOV(InitOptions.FOV)
    , AspectRatio(InitOptions.AspectRatio)
    , NearPlane(InitOptions.NearPlane)
    , FarPlane(InitOptions.FarPlane)
    , OrthoWidth(InitOptions.OrthoWidth)
    , OrthoHeight(InitOptions.OrthoHeight)
    , ViewportWidth(InitOptions.ViewportWidth)
    , ViewportHeight(InitOptions.ViewportHeight)
{
    UpdateMatrices();
}

void FSceneView::SetViewLocation(const FVector& NewLocation)
{
    if (ViewLocation != NewLocation)
    {
        ViewLocation = NewLocation;
        bViewMatrixDirty = true;
    }
}

void FSceneView::SetViewRotation(const FVector& NewRotation)
{
    if (ViewRotation != NewRotation)
    {
        ViewRotation = NewRotation;
        bViewMatrixDirty = true;
    }
}

void FSceneView::SetProjectionType(EProjectionType NewType)
{
    if (ProjectionType != NewType)
    {
        ProjectionType = NewType;
        bProjectionMatrixDirty = true;
    }
}

void FSceneView::SetFOV(float NewFOV)
{
    if (FOV != NewFOV)
    {
        FOV = NewFOV;
        bProjectionMatrixDirty = true;
    }
}

void FSceneView::SetAspectRatio(float NewAspectRatio)
{
    if (AspectRatio != NewAspectRatio)
    {
        AspectRatio = NewAspectRatio;
        bProjectionMatrixDirty = true;
    }
}

void FSceneView::SetNearFarPlanes(float NewNear, float NewFar)
{
    if (NearPlane != NewNear || FarPlane != NewFar)
    {
        NearPlane = NewNear;
        FarPlane = NewFar;
        bProjectionMatrixDirty = true;
    }
}

void FSceneView::SetOrthoSize(float Width, float Height)
{
    if (OrthoWidth != Width || OrthoHeight != Height)
    {
        OrthoWidth = Width;
        OrthoHeight = Height;
        bProjectionMatrixDirty = true;
    }
}

void FSceneView::SetViewportSize(uint32 Width, uint32 Height)
{
    if (ViewportWidth != Width || ViewportHeight != Height)
    {
        ViewportWidth = Width;
        ViewportHeight = Height;
        AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
        bProjectionMatrixDirty = true;
    }
}

const FMatrix& FSceneView::GetViewMatrix()
{
    if (bViewMatrixDirty)
    {
        UpdateViewMatrix();
        bViewMatrixDirty = false;
    }
    return ViewMatrix;
}

const FMatrix& FSceneView::GetProjectionMatrix()
{
    if (bProjectionMatrixDirty)
    {
        UpdateProjectionMatrix();
        bProjectionMatrixDirty = false;
    }
    return ProjectionMatrix;
}

const FMatrix& FSceneView::GetViewProjectionMatrix()
{
    ViewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
    return ViewProjectionMatrix;
}

FVector FSceneView::GetViewDirection() const
{
    float Pitch = FMath::DegreesToRadians(ViewRotation.X);
    float Yaw = FMath::DegreesToRadians(ViewRotation.Y);

    float CosPitch = FMath::Cos(Pitch);
    float SinPitch = FMath::Sin(Pitch);
    float CosYaw = FMath::Cos(Yaw);
    float SinYaw = FMath::Sin(Yaw);

    return FVector(
        CosPitch * CosYaw,
        CosPitch * SinYaw,
        SinPitch
    ).Normalize();
}

FVector FSceneView::GetRightVector() const
{
    FVector Forward = GetViewDirection();
    FVector Up = FVector(0.0f, 0.0f, 1.0f);
    
    return Forward.Cross(Up).Normalize();
}

FVector FSceneView::GetUpVector() const
{
    FVector Forward = GetViewDirection();
    FVector Right = GetRightVector();
    return Right.Cross(Forward).Normalize();
}

void FSceneView::UpdateMatrices()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void FSceneView::UpdateViewMatrix()
{
    FVector Forward = GetViewDirection();
    FVector Right = GetRightVector();
    FVector Up = GetUpVector();

    ViewMatrix = FMatrix::CreateLookAt(ViewLocation, ViewLocation + Forward, Up);
}

void FSceneView::UpdateProjectionMatrix()
{
    if (ProjectionType == EProjectionType::Perspective)
    {
        ProjectionMatrix = FMatrix::CreatePerspective(
            FMath::DegreesToRadians(FOV),
            AspectRatio,
            NearPlane,
            FarPlane
        );
    }
    else
    {
        ProjectionMatrix = FMatrix::CreateOrthographic(
            OrthoWidth,
            OrthoHeight,
            NearPlane,
            FarPlane
        );
    }
}