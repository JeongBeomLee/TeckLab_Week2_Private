#pragma once

using Microsoft::WRL::ComPtr;

class FD3D11GraphicsDevice
{
public:
    struct FDeviceDesc
    {
        HWND WindowHandle = nullptr;
        uint32 WindowWidth = 1920;
        uint32 WindowHeight = 1080;
        bool bFullscreen = false;
        bool bVSync = true;
        DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        uint32 MSAASamples = 1;
        uint32 MSAAQuality = 0;
    };

private:
    ComPtr<ID3D11Device> Device;
    ComPtr<ID3D11DeviceContext> Context;
    ComPtr<IDXGISwapChain> SwapChain;
    ComPtr<ID3D11RenderTargetView> BackBufferRTV;
    ComPtr<ID3D11Texture2D> DepthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> DepthStencilView;

    D3D11_VIEWPORT MainViewport;
    FDeviceDesc DeviceDesc;

    bool bInitialized = false;

public:
    FD3D11GraphicsDevice() = default;
    ~FD3D11GraphicsDevice() { Shutdown(); }

    bool Init(const FDeviceDesc& InDesc = FDeviceDesc());
    void Shutdown();

    bool IsInitialized() const { return bInitialized; }

    ID3D11Device* GetDevice() const { return Device.Get(); }
    ID3D11DeviceContext* GetContext() const { return Context.Get(); }
    IDXGISwapChain* GetSwapChain() const { return SwapChain.Get(); }

    ID3D11RenderTargetView* GetBackBufferRTV() const { return BackBufferRTV.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() const { return DepthStencilView.Get(); }
    const D3D11_VIEWPORT& GetMainViewport() const { return MainViewport; }

    const FDeviceDesc& GetDeviceDesc() const { return DeviceDesc; }

    void ClearRenderTargets(const float* ClearColor = nullptr);
    void SwapBuffer();
    void SetViewport(const D3D11_VIEWPORT& Viewport);

    bool CreateBuffer(const D3D11_BUFFER_DESC& Desc, const D3D11_SUBRESOURCE_DATA* InitialData, ID3D11Buffer** OutBuffer);
    bool CreateVertexShader(const void* ByteCode, size_t CodeSize, ID3D11VertexShader** OutShader, ID3D11InputLayout** OutInputLayout = nullptr, const D3D11_INPUT_ELEMENT_DESC* InputElements = nullptr, uint32 NumElements = 0);
    bool CreatePixelShader(const void* ByteCode, size_t CodeSize, ID3D11PixelShader** OutShader);

private:
    bool CreateDeviceAndContext();
    bool CreateSwapChain(HWND WindowHandle, uint32 Width, uint32 Height, bool bFullscreen);
    bool CreateRenderTargetView();
    bool CreateDepthStencilBuffer(uint32 Width, uint32 Height);
    bool CreateDepthStencilView();
    void SetupViewport(uint32 Width, uint32 Height);
};