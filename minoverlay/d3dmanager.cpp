#include "d3dmanager.h"
#include <d3d11.h>
#include <wrl/client.h> // ComPtr

class D3DManagerImpl
{
    Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pd3dDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> g_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_mainRenderTargetView;

public:
    bool Initialize(HWND hWnd)
    {
        UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        DXGI_SWAP_CHAIN_DESC scDesc = {};
        scDesc.BufferCount = 2;
        scDesc.BufferDesc.Width = 0;
        scDesc.BufferDesc.Height = 0;
        scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        scDesc.BufferDesc.RefreshRate.Numerator = 60;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = hWnd;
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.Windowed = TRUE;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2,
                                                D3D11_SDK_VERSION, &scDesc, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
        if (FAILED(hr))
        {
            return false;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
        if (FAILED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
        {
            return false;
        }

        if (FAILED(g_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &g_mainRenderTargetView)))
        {
            return false;
        }

        return true;
    }

    void Present()
    {
        g_pSwapChain->Present(1, 0);
    }

    // keep buffer
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
    ID3D11Texture2D *GetDrawFrame()
    {
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer));
        return m_backBuffer.Get();
    }
};

D3DManager::D3DManager()
    : m_impl(new D3DManagerImpl)
{
}

D3DManager::~D3DManager()
{
    delete m_impl;
}

bool D3DManager::Initialize(void *hWnd)
{
    return m_impl->Initialize((HWND)hWnd);
}

void *D3DManager::GetDrawFrame()
{
    return m_impl->GetDrawFrame();
}
