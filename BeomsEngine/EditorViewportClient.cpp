#include "pch.h"
#include "EditorViewportClient.h"
#include <iostream>

FEditorViewportClient::FEditorViewportClient()
    : FViewportClient(EViewportType::Editor)
{
}

void FEditorViewportClient::Initialize(uint32 Width, uint32 Height)
{
    FViewportClient::Initialize(Width, Height);

    ResetCamera();
    printf("   EditorViewportClient initialized with %s camera mode\n",
        CameraMode == ECameraMode::Orbit ? "Orbit" :
        CameraMode == ECameraMode::FirstPerson ? "FirstPerson" : "Free");
}

void FEditorViewportClient::Tick(float DeltaTime)
{
    FViewportClient::Tick(DeltaTime);

    ProcessCameraInput(DeltaTime);
}

void FEditorViewportClient::OnMouseMove(const FViewportInputEvent& Event)
{
    if (bMouseCaptured)
    {
        ProcessMouseInput(Event);
    }

    LastMousePosition = FVector(static_cast<float>(Event.MouseX), static_cast<float>(Event.MouseY), 0.0f);
}

void FEditorViewportClient::OnMouseButtonDown(const FViewportInputEvent& Event)
{
    if (Event.Button == EMouseButton::Right)
    {
        bMouseCaptured = true;
        LastMousePosition = FVector(static_cast<float>(Event.MouseX), static_cast<float>(Event.MouseY), 0.0f);
        printf("   Mouse captured for camera control\n");
    }
}

void FEditorViewportClient::OnMouseButtonUp(const FViewportInputEvent& Event)
{
    if (Event.Button == EMouseButton::Right)
    {
        bMouseCaptured = false;
        printf("   Mouse released\n");
    }
}

void FEditorViewportClient::OnKeyDown(uint32 KeyCode)
{
    switch (KeyCode)
    {
    case '1': SetCameraMode(ECameraMode::Orbit); break;
    case '2': SetCameraMode(ECameraMode::FirstPerson); break;
    case '3': SetCameraMode(ECameraMode::Free); break;
    case 'R': ResetCamera(); break;
    }
}

void FEditorViewportClient::OnKeyUp(uint32 KeyCode)
{
}

void FEditorViewportClient::SetViewMode(EEditorViewMode NewMode)
{
    if (ViewMode != NewMode)
    {
        ViewMode = NewMode;
        printf("   ViewMode changed to: %s\n",
            ViewMode == EEditorViewMode::Wireframe ? "Wireframe" :
            ViewMode == EEditorViewMode::Unlit ? "Unlit" :
            ViewMode == EEditorViewMode::Lit ? "Lit" : "VertexColor");
    }
}

void FEditorViewportClient::SetCameraMode(ECameraMode NewMode)
{
    if (CameraMode != NewMode)
    {
        CameraMode = NewMode;
        printf("   CameraMode changed to: %s\n",
            CameraMode == ECameraMode::Orbit ? "Orbit" :
            CameraMode == ECameraMode::FirstPerson ? "FirstPerson" : "Free");
    }
}

void FEditorViewportClient::SetOrbitTarget(const FVector& NewTarget)
{
    OrbitTarget = NewTarget;
    if (CameraMode == ECameraMode::Orbit)
    {
        UpdateOrbitCamera();
    }
}

void FEditorViewportClient::SetOrbitDistance(float NewDistance)
{
    OrbitDistance = FMath::Max(50.0f, NewDistance);
    if (CameraMode == ECameraMode::Orbit)
    {
        UpdateOrbitCamera();
    }
}

void FEditorViewportClient::FocusOnPoint(const FVector& Point)
{
    SetOrbitTarget(Point);
    SetOrbitDistance(500.0f);
    SetCameraMode(ECameraMode::Orbit);
}

void FEditorViewportClient::ResetCamera()
{
    OrbitTarget = FVector::Zero;
    OrbitDistance = 500.0f;
    OrbitPitch = -15.0f;
    OrbitYaw = 0.0f;

    SetCameraMode(ECameraMode::Orbit);
    UpdateOrbitCamera();

    printf("   Camera reset to default position\n");
}

void FEditorViewportClient::CreateSceneView()
{
    FSceneViewInitOptions InitOptions;
    InitOptions.ViewLocation = FVector(0.0f, -500.0f, 300.0f);
    InitOptions.ViewRotation = FVector(-15.0f, 0.0f, 0.0f);
    InitOptions.ProjectionType = EProjectionType::Perspective;
    InitOptions.FOV = 75.0f;
    InitOptions.ViewportWidth = ViewportWidth;
    InitOptions.ViewportHeight = ViewportHeight;

    SceneView = new FSceneView(InitOptions);
}

void FEditorViewportClient::UpdateSceneView(float DeltaTime)
{
    if (!SceneView)
        return;

    switch (CameraMode)
    {
    case ECameraMode::Orbit:
        UpdateOrbitCamera();
        break;
    case ECameraMode::FirstPerson:
        UpdateFirstPersonCamera();
        break;
    case ECameraMode::Free:
        UpdateFreeCamera();
        break;
    }
}

void FEditorViewportClient::UpdateOrbitCamera()
{
    if (!SceneView)
        return;

    FVector CameraPosition = CalculateOrbitPosition();
    FVector LookDirection = (OrbitTarget - CameraPosition).Normalize();

    float Pitch = FMath::RadiansToDegrees(asin(LookDirection.Z));
    float Yaw = FMath::RadiansToDegrees(atan2(LookDirection.Y, LookDirection.X));

    SceneView->SetViewLocation(CameraPosition);
    SceneView->SetViewRotation(FVector(Pitch, Yaw, 0.0f));
}

void FEditorViewportClient::UpdateFirstPersonCamera()
{
    if (!SceneView)
        return;
}

void FEditorViewportClient::UpdateFreeCamera()
{
    if (!SceneView)
        return;
}

void FEditorViewportClient::ProcessCameraInput(float DeltaTime)
{
}

void FEditorViewportClient::ProcessMouseInput(const FViewportInputEvent& Event)
{
    if (!bMouseCaptured)
        return;

    float DeltaX = static_cast<float>(Event.MouseDeltaX) * MouseSensitivity;
    float DeltaY = static_cast<float>(Event.MouseDeltaY) * MouseSensitivity;

    if (CameraMode == ECameraMode::Orbit)
    {
        OrbitYaw += DeltaX * 0.5f;
        OrbitPitch += DeltaY * 0.5f;

        OrbitPitch = FMath::Clamp(OrbitPitch, -89.0f, 89.0f);

        while (OrbitYaw > 360.0f) OrbitYaw -= 360.0f;
        while (OrbitYaw < 0.0f) OrbitYaw += 360.0f;
    }
}

FVector FEditorViewportClient::CalculateOrbitPosition() const
{
    float PitchRad = FMath::DegreesToRadians(OrbitPitch);
    float YawRad = FMath::DegreesToRadians(OrbitYaw);

    float X = OrbitDistance * FMath::Cos(PitchRad) * FMath::Cos(YawRad);
    float Y = OrbitDistance * FMath::Cos(PitchRad) * FMath::Sin(YawRad);
    float Z = OrbitDistance * FMath::Sin(PitchRad);

    return OrbitTarget + FVector(X, Y, Z);
}