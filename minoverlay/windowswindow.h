#pragma once
#include <string>

class WindowsWindow
{
    class WindowsWindowImpl *m_impl = nullptr;

public:
    WindowsWindow();
    ~WindowsWindow();
    void* Initialize(const wchar_t *className, const wchar_t *windowName);
    bool Loop();
    void Wait();
};

std::string GetExePath();
