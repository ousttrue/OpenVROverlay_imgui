#pragma once

class WindowsWindow
{
    class WindowsWindowImpl *m_impl = nullptr;

public:
    WindowsWindow();
    ~WindowsWindow();
    void* Initialize(void *hInstance, const wchar_t *className, const wchar_t *windowName);
    bool Loop();
};
