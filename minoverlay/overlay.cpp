#include "overlay.h"
#include <stdint.h>
#include <openvr.h>
#include "windowswindow.h"

class OverlayImpl
{
    uint64_t g_Time = 0;
    uint64_t g_TicksPerSecond = 0;

    vr::VROverlayHandle_t g_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;
    vr::VROverlayHandle_t g_ulOverlayThumbnailHandle = vr::k_ulOverlayHandleInvalid;

public:
    bool InitOpenVR()
    {
        vr::EVRInitError initError = vr::VRInitError_None;
        vr::VR_Init(&initError, vr::VRApplication_Overlay);

        if (!vr::VROverlay())
            return false;

        vr::VROverlay()->ClearOverlayTexture(g_ulOverlayHandle);
        vr::VROverlay()->DestroyOverlay(g_ulOverlayHandle);

        g_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;

        std::string name = "Hoge";
        vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(name.c_str(), name.c_str(), &g_ulOverlayHandle, &g_ulOverlayThumbnailHandle);
        if (overlayError != vr::VROverlayError_None)
        {
            if (overlayError == vr::VROverlayError_KeyInUse)
            {
            }
            return false;
        }

        vr::VROverlay()->SetOverlayAlpha(g_ulOverlayHandle, 1.0f);
        vr::VROverlay()->SetOverlayColor(g_ulOverlayHandle, 1.0f, 1.0f, 1.0f);

        vr::VROverlay()->SetOverlayWidthInMeters(g_ulOverlayHandle, 2.0f);
        vr::VROverlay()->SetOverlayInputMethod(g_ulOverlayHandle, vr::VROverlayInputMethod_Mouse);

        std::string path = GetExePath();
        std::string thumbIconPath = path + "\\..\\..\\x64\\Debug\\thumbicon.png";
        overlayError = vr::VROverlay()->SetOverlayFromFile(g_ulOverlayThumbnailHandle, thumbIconPath.c_str());
        if (overlayError != vr::VROverlayError_None)
        {
            return false;
        }

        return true;
    }

    // 毎フレーム処理・描画
    void RenderOverlay(void *backBuffer)
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

        vr::Texture_t texture = {backBuffer, vr::TextureType_DirectX, vr::ColorSpace_Auto};
        vr::VROverlay()->SetOverlayTexture(g_ulOverlayHandle, &texture);
    }
};

Overlay::Overlay()
: m_impl(new OverlayImpl)
{
}

Overlay::~Overlay()
{
    delete m_impl;
}

bool Overlay::Initialize(void *hWnd)
{
    return m_impl->InitOpenVR();
}

void Overlay::Render(void *d3dtexture)
{
    m_impl->RenderOverlay(d3dtexture);
}
