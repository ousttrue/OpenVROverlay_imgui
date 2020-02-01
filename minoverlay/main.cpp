#include "windowswindow.h"
#include "d3dmanager.h"
#include "overlay.h"

auto CLASS_NAME = L"MinOverlayClass";
auto WINDOW_NAME = L"MinOverlay";

int main(int argc, char **argv)
{
    WindowsWindow window;
    auto hWnd = window.Initialize(CLASS_NAME, WINDOW_NAME);
    if (!hWnd)
    {
        return 1;
    }

    D3DManager d3d;
    if (!d3d.Initialize(hWnd))
    {
        return 2;
    }

    Overlay overlay;
    if (!overlay.Initialize("minoverlay", "minimum overlay"))
    {
        return 3;
    }

    while (window.Loop())
    {
        auto texture = d3d.GetDrawFrame();
        overlay.Render(texture);
        window.Wait();
    }

    return 0;
}
