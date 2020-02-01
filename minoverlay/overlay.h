#pragma once


class Overlay
{
    class OverlayImpl *m_impl = nullptr;
public:
    Overlay();
    ~Overlay();
    bool Initialize(void *hWnd);
    void Render(void *d3dtexture);
};
