#include "pch.h"
#include "RenderPass.h"

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
}