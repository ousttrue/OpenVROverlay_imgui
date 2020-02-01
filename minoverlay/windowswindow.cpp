#include "windowswindow.h"
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        auto hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

class WindowsWindowImpl
{
public:
    HWND Initialize(HINSTANCE hInstance, const wchar_t *className, const wchar_t *windowName)
    {
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
        wcex.lpszClassName = className;
        wcex.hIconSm = NULL;
        if (!RegisterClassEx(&wcex))
        {
            return nullptr;
        }

        RECT rc = {0, 0, 512, 512};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        auto hWnd = CreateWindowW(className, windowName, WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                                  NULL, NULL, hInstance, NULL);
        if (!hWnd)
        {
            return nullptr;
        }

        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);

        return hWnd;
    }

    bool Loop()
    {
        MSG msg = {};
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }
};

WindowsWindow::WindowsWindow()
    : m_impl(new WindowsWindowImpl)
{
}

WindowsWindow::~WindowsWindow()
{
    delete m_impl;
}

void *WindowsWindow::Initialize(void *hInstance, const wchar_t *className, const wchar_t *windowName)
{
    return m_impl->Initialize((HINSTANCE)hInstance, className, windowName);
}

bool WindowsWindow::Loop()
{
    return m_impl->Loop();
}
