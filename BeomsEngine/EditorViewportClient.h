#pragma once
#include "ViewportClient.h"

enum class EEditorViewMode
{
    Wireframe,
    Unlit,
    Lit,
    VertexColor
};

enum class ECameraMode
{
    Orbit,
    FirstPerson,
    Free
};

class FEditorViewportClient : public FViewportClient
{
private:
    EEditorViewMode ViewMode = EEditorViewMode::Lit;
    ECameraMode CameraMode = ECameraMode::Orbit;

    FVector OrbitTarget = FVector::Zero;
    float OrbitDistance = 500.0f;
    float OrbitPitch = -15.0f;
    float OrbitYaw = 0.0f;

    float CameraSpeed = 300.0f;
    float MouseSensitivity = 1.0f;

    bool bMouseCaptured = false;
    FVector LastMousePosition = FVector::Zero;

public:
    FEditorViewportClient();
    virtual ~FEditorViewportClient() = default;

    virtual void Initialize(uint32 Width, uint32 Height) override;
    virtual void Tick(float DeltaTime) override;

    virtual void OnMouseMove(const FViewportInputEvent& Event) override;
    virtual void OnMouseButtonDown(const FViewportInputEvent& Event) override;
    virtual void OnMouseButtonUp(const FViewportInputEvent& Event) override;
    virtual void OnKeyDown(uint32 KeyCode) override;
    virtual void OnKeyUp(uint32 KeyCode) override;

    void SetViewMode(EEditorViewMode NewMode);
    void SetCameraMode(ECameraMode NewMode);
    void SetOrbitTarget(const FVector& NewTarget);
    void SetOrbitDistance(float NewDistance);
    void SetCameraSpeed(float NewSpeed) { CameraSpeed = NewSpeed; }
    void SetMouseSensitivity(float NewSensitivity) { MouseSensitivity = NewSensitivity; }

    EEditorViewMode GetViewMode() const { return ViewMode; }
    ECameraMode GetCameraMode() const { return CameraMode; }
    const FVector& GetOrbitTarget() const { return OrbitTarget; }
    float GetOrbitDistance() const { return OrbitDistance; }

    void FocusOnPoint(const FVector& Point);
    void ResetCamera();

protected:
    virtual void CreateSceneView() override;
    virtual void UpdateSceneView(float DeltaTime) override;

private:
    void UpdateOrbitCamera();
    void UpdateFirstPersonCamera();
    void UpdateFreeCamera();

    void ProcessCameraInput(float DeltaTime);
    void ProcessMouseInput(const FViewportInputEvent& Event);

    FVector CalculateOrbitPosition() const;
};