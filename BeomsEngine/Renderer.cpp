#include "pch.h"
#include "Renderer.h"
#include "D3D11GraphicsDevice.h"

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
}

void URenderer::RenderScene()
{
    RenderSceneWithView(nullptr);
}

void URenderer::RenderSceneWithView(FSceneView* SceneView)
{
    if (!bInitialized || !DeviceContext || !GraphicsDevice)
    {
        return;
    }

    FRenderPassContext Context;
    Context.DeviceContext = DeviceContext;
    Context.RenderTarget = GraphicsDevice->GetBackBufferRTV();
    Context.DepthStencil = GraphicsDevice->GetDepthStencilView();
    Context.Viewport = &GraphicsDevice->GetMainViewport();
    Context.SceneView = SceneView;

    for (IRenderPass* Pass : RenderPasses)
    {
        if (Pass)
        {
            Context.PassType = Pass->GetPassType();
            ExecuteRenderPass(Pass, Context);
        }
    }
}

void URenderer::AddRenderPass(IRenderPass* Pass)
{
    if (Pass)
    {
        RenderPasses.push_back(Pass);
    }
}

void URenderer::RemoveRenderPass(ERenderPassType PassType)
{
    for (auto it = RenderPasses.begin(); it != RenderPasses.end(); ++it)
    {
        if ((*it) && (*it)->GetPassType() == PassType)
        {
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

    Pass->Execute(Context);
}