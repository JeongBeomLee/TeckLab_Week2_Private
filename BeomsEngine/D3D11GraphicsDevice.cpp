#include "pch.h"
#include "D3D11GraphicsDevice.h"

bool FD3D11GraphicsDevice::Init(const FDeviceDesc& InDesc)
{
    if (bInitialized)
    {
        return true;
    }

    DeviceDesc = InDesc;

    if (!CreateDeviceAndContext())
    {
        return false;
    }

    if (!CreateSwapChain(DeviceDesc.WindowHandle, DeviceDesc.WindowWidth, DeviceDesc.WindowHeight, DeviceDesc.bFullscreen))
    {
        return false;
    }

    if (!CreateRenderTargetView())
    {
        return false;
    }

    if (!CreateDepthStencilBuffer(DeviceDesc.WindowWidth, DeviceDesc.WindowHeight))
    {
        return false;
    }

    if (!CreateDepthStencilView())
    {
        return false;
    }

    SetupViewport(DeviceDesc.WindowWidth, DeviceDesc.WindowHeight);

    bInitialized = true;
    return true;
}

void FD3D11GraphicsDevice::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    DepthStencilView.Reset();
    DepthStencilBuffer.Reset();
    BackBufferRTV.Reset();
    SwapChain.Reset();
    Context.Reset();
    Device.Reset();

    bInitialized = false;
}

bool FD3D11GraphicsDevice::CreateDeviceAndContext()
{
    D3D_FEATURE_LEVEL FeatureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL FeatureLevel;
    UINT CreateDeviceFlags = 0;

#ifdef _DEBUG
    CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        CreateDeviceFlags,
        FeatureLevels,
        ARRAYSIZE(FeatureLevels),
        D3D11_SDK_VERSION,
        Device.GetAddressOf(),
        &FeatureLevel,
        Context.GetAddressOf()
    );

    return SUCCEEDED(hr);
}

bool FD3D11GraphicsDevice::CreateSwapChain(HWND WindowHandle, uint32 Width, uint32 Height, bool bFullscreen)
{
    ComPtr<IDXGIDevice> DxgiDevice;
    if (FAILED(Device.As(&DxgiDevice)))
    {
        return false;
    }

    ComPtr<IDXGIAdapter> DxgiAdapter;
    if (FAILED(DxgiDevice->GetAdapter(DxgiAdapter.GetAddressOf())))
    {
        return false;
    }

    ComPtr<IDXGIFactory> DxgiFactory;
    if (FAILED(DxgiAdapter->GetParent(IID_PPV_ARGS(DxgiFactory.GetAddressOf()))))
    {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = Width;
    SwapChainDesc.BufferDesc.Height = Height;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = DeviceDesc.BackBufferFormat;
    SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    SwapChainDesc.SampleDesc.Count = DeviceDesc.MSAASamples;
    SwapChainDesc.SampleDesc.Quality = DeviceDesc.MSAAQuality;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.OutputWindow = WindowHandle;
    SwapChainDesc.Windowed = !bFullscreen;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.Flags = 0;

    HRESULT hr = DxgiFactory->CreateSwapChain(Device.Get(), &SwapChainDesc, SwapChain.GetAddressOf());
    return SUCCEEDED(hr);
}

bool FD3D11GraphicsDevice::CreateRenderTargetView()
{
    ComPtr<ID3D11Texture2D> BackBuffer;
    if (FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(BackBuffer.GetAddressOf()))))
    {
        return false;
    }

    HRESULT hr = Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, BackBufferRTV.GetAddressOf());
    return SUCCEEDED(hr);
}

bool FD3D11GraphicsDevice::CreateDepthStencilBuffer(uint32 Width, uint32 Height)
{
    D3D11_TEXTURE2D_DESC DepthStencilDesc = {};
    DepthStencilDesc.Width = Width;
    DepthStencilDesc.Height = Height;
    DepthStencilDesc.MipLevels = 1;
    DepthStencilDesc.ArraySize = 1;
    DepthStencilDesc.Format = DeviceDesc.DepthStencilFormat;
    DepthStencilDesc.SampleDesc.Count = DeviceDesc.MSAASamples;
    DepthStencilDesc.SampleDesc.Quality = DeviceDesc.MSAAQuality;
    DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DepthStencilDesc.CPUAccessFlags = 0;
    DepthStencilDesc.MiscFlags = 0;

    HRESULT hr = Device->CreateTexture2D(&DepthStencilDesc, nullptr, DepthStencilBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

bool FD3D11GraphicsDevice::CreateDepthStencilView()
{
    HRESULT hr = Device->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, DepthStencilView.GetAddressOf());
    return SUCCEEDED(hr);
}

void FD3D11GraphicsDevice::SetupViewport(uint32 Width, uint32 Height)
{
    MainViewport.TopLeftX = 0.0f;
    MainViewport.TopLeftY = 0.0f;
    MainViewport.Width = static_cast<float>(Width);
    MainViewport.Height = static_cast<float>(Height);
    MainViewport.MinDepth = 0.0f;
    MainViewport.MaxDepth = 1.0f;

    Context->RSSetViewports(1, &MainViewport);
}

void FD3D11GraphicsDevice::ClearRenderTargets(const float* ClearColor)
{
    static const float DefaultClearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    const float* Color = ClearColor ? ClearColor : DefaultClearColor;

    Context->ClearRenderTargetView(BackBufferRTV.Get(), Color);
    Context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FD3D11GraphicsDevice::SwapBuffer()
{
    SwapChain->Present(DeviceDesc.bVSync ? 1 : 0, 0);
}

void FD3D11GraphicsDevice::SetViewport(const D3D11_VIEWPORT& Viewport)
{
    Context->RSSetViewports(1, &Viewport);
}

bool FD3D11GraphicsDevice::CreateBuffer(const D3D11_BUFFER_DESC& Desc, const D3D11_SUBRESOURCE_DATA* InitialData, ID3D11Buffer** OutBuffer)
{
    HRESULT hr = Device->CreateBuffer(&Desc, InitialData, OutBuffer);
    return SUCCEEDED(hr);
}

bool FD3D11GraphicsDevice::CreateVertexShader(const void* ByteCode, size_t CodeSize, ID3D11VertexShader** OutShader, ID3D11InputLayout** OutInputLayout, const D3D11_INPUT_ELEMENT_DESC* InputElements, uint32 NumElements)
{
    HRESULT hr = Device->CreateVertexShader(ByteCode, CodeSize, nullptr, OutShader);
    if (FAILED(hr))
    {
        return false;
    }

    if (OutInputLayout && InputElements && NumElements > 0)
    {
        hr = Device->CreateInputLayout(InputElements, NumElements, ByteCode, CodeSize, OutInputLayout);
        if (FAILED(hr))
        {
            (*OutShader)->Release();
            *OutShader = nullptr;
            return false;
        }
    }

    return true;
}

bool FD3D11GraphicsDevice::CreatePixelShader(const void* ByteCode, size_t CodeSize, ID3D11PixelShader** OutShader)
{
    HRESULT hr = Device->CreatePixelShader(ByteCode, CodeSize, nullptr, OutShader);
    return SUCCEEDED(hr);
}