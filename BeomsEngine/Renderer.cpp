#include "pch.h"
#include "Renderer.h"
#include "D3D11GraphicsDevice.h"
#include <iostream>

IMPLEMENT_CLASS(URenderer, UObject)

URenderer::URenderer(ID3D11DeviceContext* InDeviceContext, FD3D11GraphicsDevice* InGraphicsDevice)
{
    InitializeRenderer(InDeviceContext, InGraphicsDevice);
}

URenderer::~URenderer()
{
    Shutdown();
}

void URenderer::InitializeRenderer(ID3D11DeviceContext* InDeviceContext, FD3D11GraphicsDevice* InGraphicsDevice)
{
    if (bInitialized)
    {
        return;
    }

    DeviceContext = InDeviceContext;
    GraphicsDevice = InGraphicsDevice;

    SetupDefaultRenderPasses();

    bInitialized = true;
    printf("    URenderer initialized with %zu render passes\n", RenderPasses.size());
}

void URenderer::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    ClearRenderPasses();
    DeviceContext = nullptr;
    GraphicsDevice = nullptr;
    bInitialized = false;
    MarkPendingKill();

    printf("    URenderer shutdown completed\n");
}

void URenderer::RenderScene()
{
    if (!bInitialized || !DeviceContext || !GraphicsDevice)
    {
        printf("    URenderer not properly initialized\n");
        return;
    }

    printf("   >> Starting Scene Rendering (%zu passes)\n", RenderPasses.size());

    FRenderPassContext Context;
    Context.DeviceContext = DeviceContext;
    Context.RenderTarget = GraphicsDevice->GetBackBufferRTV();
    Context.DepthStencil = GraphicsDevice->GetDepthStencilView();
    Context.Viewport = &GraphicsDevice->GetMainViewport();

    for (IRenderPass* Pass : RenderPasses)
    {
        if (Pass)
        {
            Context.PassType = Pass->GetPassType();
            ExecuteRenderPass(Pass, Context);
        }
    }

    printf("   >> Scene Rendering Completed\n");
}

void URenderer::AddRenderPass(IRenderPass* Pass)
{
    if (Pass)
    {
        RenderPasses.push_back(Pass);
        printf("   + Added render pass: %s\n", Pass->GetPassName());
    }
}

void URenderer::RemoveRenderPass(ERenderPassType PassType)
{
    for (auto it = RenderPasses.begin(); it != RenderPasses.end(); ++it)
    {
        if ((*it) && (*it)->GetPassType() == PassType)
        {
            printf("   - Removed render pass: %s\n", (*it)->GetPassName());
            delete (*it);
            RenderPasses.erase(it);
            return;
        }
    }
}

void URenderer::ClearRenderPasses()
{
    for (IRenderPass* Pass : RenderPasses)
    {
        if (Pass)
        {
            delete Pass;
        }
    }
    RenderPasses.clear();
    printf("   - All render passes cleared\n");
}

void URenderer::SetupDefaultRenderPasses()
{
    AddRenderPass(new FDepthPrePass());
    AddRenderPass(new FBasePass());

    //AddRenderPass(new FUIPass());
}

void URenderer::ExecuteRenderPass(IRenderPass* Pass, const FRenderPassContext& Context)
{
    if (!Pass)
    {
        return;
    }

    printf("   Executing: %s\n", Pass->GetPassName());
    Pass->Execute(Context);
}