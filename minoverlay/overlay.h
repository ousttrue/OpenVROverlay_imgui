#pragma once

class Overlay
{
    class OverlayImpl *m_impl = nullptr;

public:
    Overlay();
    ~Overlay();
    bool Initialize(const char *key, const char *name);
    void Render(void *d3dtexture);
};
