#include <windows.h>
#include <d3d11.h>
#include <openvr.h>

/////////////////////////////////////////
template <typename LPD3D>
void SAFE_RELEASE(LPD3D &p)
{
    if (p)
        p->Release();
    p = nullptr;
}

/////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool Initialize(HWND hWnd);
HRESULT CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
bool InitOpenVR();
void RenderOverlay(HWND hWnd);


ID3D11Device *g_pd3dDevice = NULL;
ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
IDXGISwapChain *g_pSwapChain = NULL;
ID3D11RenderTargetView *g_mainRenderTargetView = NULL;

vr::VROverlayHandle_t g_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;
vr::VROverlayHandle_t g_ulOverlayThumbnailHandle = vr::k_ulOverlayHandleInvalid;

INT64 g_Time = 0;
INT64 g_TicksPerSecond = 0;

std::string GetExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

/////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // ウインドウ作成
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Dashboard";
    wcex.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }
    RECT rc = {0, 0, 512, 512};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindow(L"Dashboard", L"Dashboard", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                             NULL, NULL, hInstance, NULL);
    if (!hWnd)
    {
        return 0;
    }
    ///ShowWindow( hWnd, nCmdShow );
    ///UpdateWindow( hWnd );

    // イベントループ
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        RenderOverlay(hWnd);

        Sleep(10);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
        if (!Initialize(hWnd))
        {
            CleanupDeviceD3D();

            PostQuitMessage(0);
        }
        break;
    case WM_SYSCOMMAND:
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 初期化入り口
bool Initialize(HWND hWnd)
{
    // Direct3Dの初期化
    if (CreateDeviceD3D(hWnd) != S_OK)
    {
        return false;
    }

    // OpenVRの初期化
    if (!InitOpenVR())
    {
        return false;
    }

    return true;
}

// Direct3Dの初期化
HRESULT CreateDeviceD3D(HWND hWnd)
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

    // デバイスコンテキストとスワップチェーンを作成
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2,
                                               D3D11_SDK_VERSION, &scDesc, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (hr != S_OK)
    {
        return E_FAIL;
    }

    // レンダーターゲット
    ID3D11Texture2D *pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);

    pBackBuffer->Release();

    return S_OK;
}

void CleanupDeviceD3D()
{
    SAFE_RELEASE(g_mainRenderTargetView);
    SAFE_RELEASE(g_pSwapChain);
    SAFE_RELEASE(g_pd3dDeviceContext);
    SAFE_RELEASE(g_pd3dDevice);
}

// OpenVRの初期化
bool InitOpenVR()
{
    vr::EVRInitError initError = vr::VRInitError_None;
    vr::VR_Init(&initError, vr::VRApplication_Overlay);

    if (!vr::VROverlay())
        return false;

    vr::VROverlay()->ClearOverlayTexture(g_ulOverlayHandle);
    vr::VROverlay()->DestroyOverlay(g_ulOverlayHandle);

    g_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;

    // OpenVR オーバーレイダッシュボード作成
    std::string name = "Hoge";
    vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(name.c_str(), name.c_str(), &g_ulOverlayHandle, &g_ulOverlayThumbnailHandle);
    if (overlayError != vr::VROverlayError_None)
    {
        if (overlayError == vr::VROverlayError_KeyInUse)
        {
            // 起動済み
        }
        // エラー処理
        return false;
    }

    vr::VROverlay()->SetOverlayAlpha(g_ulOverlayHandle, 1.0f); /// 透明度 TODO:
    vr::VROverlay()->SetOverlayColor(g_ulOverlayHandle, 1.0f, 1.0f, 1.0f);

    vr::VROverlay()->SetOverlayWidthInMeters(g_ulOverlayHandle, 2.0f); /// 拡大率
    vr::VROverlay()->SetOverlayInputMethod(g_ulOverlayHandle, vr::VROverlayInputMethod_Mouse);

    // ボタンオーバーレイ作成
    std::string path = GetExePath();
    std::string thumbIconPath = path + "\\thumbicon.png"; /// ボタンのアイコン
    overlayError = vr::VROverlay()->SetOverlayFromFile(g_ulOverlayThumbnailHandle, thumbIconPath.c_str());
    if (overlayError != vr::VROverlayError_None)
    {
        // エラー処理
        return false;
    }

    return true;
}

// 毎フレーム処理・描画
void RenderOverlay(HWND hWnd)
{
    if (!vr::VROverlay() || !vr::VROverlay()->IsOverlayVisible(g_ulOverlayHandle))
        return;

    // OpenVRイベント処理
    vr::VREvent_t Event;
    while (vr::VROverlay()->PollNextOverlayEvent(g_ulOverlayHandle, &Event, sizeof(Event)))
    {
        switch (Event.eventType)
        {
        case vr::VREvent_MouseMove:
            break;
        case vr::VREvent_MouseButtonDown:
            break;
        }
    }

    g_pSwapChain->Present(1, 0);

    // オーバーレイに複写
    ID3D11Texture2D *backBuffer = NULL;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    vr::Texture_t texture = {(void *)backBuffer, vr::TextureType_DirectX, vr::ColorSpace_Auto};
    vr::VROverlay()->SetOverlayTexture(g_ulOverlayHandle, &texture);

    backBuffer->Release();
}
