#pragma once

class D3DManager
{
    class D3DManagerImpl *m_impl = nullptr;

public:
    D3DManager();
    ~D3DManager();
    bool Initialize(void *hWnd);
    void* GetDrawFrame();
};
