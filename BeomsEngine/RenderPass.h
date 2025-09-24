#pragma once
#include "Types.h"

#include <d3d11.h>

enum class ERenderPassType
{
    DepthPrePass,
    BasePass
};

struct FRenderPassContext
{
    ERenderPassType PassType;
    ID3D11DeviceContext* DeviceContext = nullptr;
    ID3D11RenderTargetView* RenderTarget = nullptr;
    ID3D11DepthStencilView* DepthStencil = nullptr;
    const D3D11_VIEWPORT* Viewport = nullptr;
    float ClearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
};

class IRenderPass
{
public:
    virtual ~IRenderPass() = default;
    virtual void Execute(const FRenderPassContext& Context) = 0;
    virtual ERenderPassType GetPassType() const = 0;
    virtual const char* GetPassName() const = 0;
};

class FDepthPrePass : public IRenderPass
{
public:
    virtual void Execute(const FRenderPassContext& Context) override;
    virtual ERenderPassType GetPassType() const override { return ERenderPassType::DepthPrePass; }
    virtual const char* GetPassName() const override { return "DepthPrePass"; }
};

class FBasePass : public IRenderPass
{
public:
    virtual void Execute(const FRenderPassContext& Context) override;
    virtual ERenderPassType GetPassType() const override { return ERenderPassType::BasePass; }
    virtual const char* GetPassName() const override { return "BasePass"; }
};