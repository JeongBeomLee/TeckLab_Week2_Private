#include "pch.h"
#include "ViewportClient.h"
#include "D3D11GraphicsDevice.h"
#include "Renderer.h"
#include <iostream>

FViewportClient::FViewportClient(EViewportType InViewportType)
    : ViewportType(InViewportType)
{
    CreateSceneView();
}

FViewportClient::~FViewportClient()
{
    Shutdown();
}

void FViewportClient::Initialize(uint32 Width, uint32 Height)
{
    ViewportWidth = Width;
    ViewportHeight = Height;

    if (SceneView)
    {
        SceneView->SetViewportSize(Width, Height);
        SceneView->UpdateMatrices();
    }

    bIsActive = true;
    printf("   ViewportClient initialized: %dx%d\n", Width, Height);
}

void FViewportClient::Shutdown()
{
    if (SceneView)
    {
        delete SceneView;
        SceneView = nullptr;
    }

    bIsActive = false;
    printf("   ViewportClient shutdown completed\n");
}

void FViewportClient::Draw(FD3D11GraphicsDevice* GraphicsDevice, URenderer* Renderer)
{
    if (!bIsActive || !SceneView || !GraphicsDevice || !Renderer)
    {
        return;
    }

    Renderer->RenderSceneWithView(SceneView);
}

void FViewportClient::Tick(float DeltaTime)
{
    if (!bIsActive)
    {
        return;
    }

    UpdateSceneView(DeltaTime);
}

void FViewportClient::SetViewLocation(const FVector& NewLocation)
{
    if (SceneView)
    {
        SceneView->SetViewLocation(NewLocation);
    }
}

void FViewportClient::SetViewRotation(const FVector& NewRotation)
{
    if (SceneView)
    {
        SceneView->SetViewRotation(NewRotation);
    }
}

void FViewportClient::SetProjectionType(EProjectionType NewType)
{
    if (SceneView)
    {
        SceneView->SetProjectionType(NewType);
    }
}

void FViewportClient::SetFOV(float NewFOV)
{
    if (SceneView)
    {
        SceneView->SetFOV(NewFOV);
    }
}

void FViewportClient::SetViewportSize(uint32 Width, uint32 Height)
{
    ViewportWidth = Width;
    ViewportHeight = Height;

    if (SceneView)
    {
        SceneView->SetViewportSize(Width, Height);
    }
}

void FViewportClient::CreateSceneView()
{
    FSceneViewInitOptions InitOptions;
    InitOptions.ViewLocation = FVector(0.0f, -500.0f, 300.0f);
    InitOptions.ViewRotation = FVector(-15.0f, 0.0f, 0.0f);
    InitOptions.ProjectionType = EProjectionType::Perspective;
    InitOptions.FOV = 90.0f;
    InitOptions.ViewportWidth = ViewportWidth;
    InitOptions.ViewportHeight = ViewportHeight;

    SceneView = new FSceneView(InitOptions);
}