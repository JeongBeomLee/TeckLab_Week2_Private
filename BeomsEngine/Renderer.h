#pragma once
#include "Types.h"
#include "Object.h"
#include "Containers.h"
#include "RenderPass.h"

class FD3D11GraphicsDevice;

class URenderer : public UObject
{
    UCLASS()
    GENERATED_BODY(URenderer, UObject)

private:
    ID3D11DeviceContext* DeviceContext = nullptr;
    FD3D11GraphicsDevice* GraphicsDevice = nullptr;
    TArray<IRenderPass*> RenderPasses;

    bool bInitialized = false;

public:
    URenderer() = default;
    URenderer(ID3D11DeviceContext* InDeviceContext, FD3D11GraphicsDevice* InGraphicsDevice);
    virtual ~URenderer();

    void InitializeRenderer(ID3D11DeviceContext* InDeviceContext, FD3D11GraphicsDevice* InGraphicsDevice);
    void Shutdown();

    void RenderScene();
    void RenderSceneWithView(FSceneView* SceneView);

    void AddRenderPass(IRenderPass* Pass);
    void RemoveRenderPass(ERenderPassType PassType);
    void ClearRenderPasses();

    bool IsInitialized() const { return bInitialized; }
    ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }
    FD3D11GraphicsDevice* GetGraphicsDevice() const { return GraphicsDevice; }

private:
    void SetupDefaultRenderPasses();
    void ExecuteRenderPass(IRenderPass* Pass, const FRenderPassContext& Context);
};