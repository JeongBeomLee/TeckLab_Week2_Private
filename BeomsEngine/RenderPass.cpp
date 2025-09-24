#include "pch.h"
#include "RenderPass.h"
#include <iostream>

void FDepthPrePass::Execute(const FRenderPassContext& Context)
{
    if (!Context.DeviceContext || !Context.DepthStencil)
    {
        return;
    }

    Context.DeviceContext->ClearDepthStencilView(
        Context.DepthStencil,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    ID3D11RenderTargetView* NullRTV = nullptr;
    Context.DeviceContext->OMSetRenderTargets(1, &NullRTV, Context.DepthStencil);

    if (Context.Viewport)
    {
        Context.DeviceContext->RSSetViewports(1, Context.Viewport);
    }

    printf("   - DepthPrePass: Depth buffer cleared and set for Z-only rendering\n");
}

void FBasePass::Execute(const FRenderPassContext& Context)
{
    if (!Context.DeviceContext || !Context.RenderTarget)
    {
        return;
    }

    Context.DeviceContext->ClearRenderTargetView(Context.RenderTarget, Context.ClearColor);

    Context.DeviceContext->OMSetRenderTargets(1, &Context.RenderTarget, Context.DepthStencil);

    if (Context.Viewport)
    {
        Context.DeviceContext->RSSetViewports(1, Context.Viewport);
    }

    printf("   - BasePass: Render targets set and cleared (Color: %.2f, %.2f, %.2f, %.2f)\n",
        Context.ClearColor[0], Context.ClearColor[1], Context.ClearColor[2], Context.ClearColor[3]);
}