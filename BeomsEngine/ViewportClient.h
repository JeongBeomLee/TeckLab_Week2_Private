#pragma once
#include "Types.h"
#include "Object.h"
#include "SceneView.h"

class FD3D11GraphicsDevice;
class URenderer;

enum class EViewportType
{
    Game,
    Editor,
    Preview
};

enum class EMouseButton
{
    Left,
    Right,
    Middle
};

struct FViewportInputEvent
{
    int32 MouseX = 0;
    int32 MouseY = 0;
    int32 MouseDeltaX = 0;
    int32 MouseDeltaY = 0;
    EMouseButton Button = EMouseButton::Left;
    bool bIsPressed = false;
    bool bCtrlPressed = false;
    bool bShiftPressed = false;
    bool bAltPressed = false;
};

class FViewportClient
{
protected:
    FSceneView* SceneView = nullptr;
    EViewportType ViewportType = EViewportType::Game;
    uint32 ViewportWidth = 1920;
    uint32 ViewportHeight = 1080;
    bool bIsActive = false;

public:
    FViewportClient(EViewportType InViewportType = EViewportType::Game);
    virtual ~FViewportClient();

    virtual void Initialize(uint32 Width, uint32 Height);
    virtual void Shutdown();

    virtual void Draw(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer);
    virtual void Tick(float DeltaTime);

    virtual void OnMouseMove(const FViewportInputEvent& Event) {}
    virtual void OnMouseButtonDown(const FViewportInputEvent& Event) {}
    virtual void OnMouseButtonUp(const FViewportInputEvent& Event) {}
    virtual void OnKeyDown(uint32 KeyCode) {}
    virtual void OnKeyUp(uint32 KeyCode) {}

    virtual void SetViewLocation(const FVector& NewLocation);
    virtual void SetViewRotation(const FVector& NewRotation);
    virtual void SetProjectionType(EProjectionType NewType);
    virtual void SetFOV(float NewFOV);

    FSceneView* GetSceneView() const { return SceneView; }
    EViewportType GetViewportType() const { return ViewportType; }
    uint32 GetViewportWidth() const { return ViewportWidth; }
    uint32 GetViewportHeight() const { return ViewportHeight; }
    bool IsActive() const { return bIsActive; }

    void SetActive(bool bActive) { bIsActive = bActive; }
    void SetViewportSize(uint32 Width, uint32 Height);

protected:
    virtual void CreateSceneView();
    virtual void UpdateSceneView(float DeltaTime) {}
};