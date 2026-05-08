#include "Defines.h"
#include "Window.h"
#ifdef WINDOWS
int Window::Create(void) {
    char CLASS_NAME[] = "Vulkan";
    char WINDOW_NAME[] = "VulkanEngine";
    hInstance = GetModuleHandleA(nullptr);
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;    // Set window procedure
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // Check if class was registered
    if (!RegisterClassA(&wc) )
        return -1;

    pWindow = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    // Create Window
    hwnd = CreateWindowExA(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class name
        WINDOW_NAME,                    // Window title
        WS_OVERLAPPEDWINDOW,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Position
        width, height,                  // Size
        nullptr,                        // Parent window
        nullptr,                        // Menu
        hInstance,                      // Instance handle
        this                            // Additional application data
    );
    if (hwnd == nullptr)
        return -1;
    this->isRunning = true;
    return 0;
}

void Window::WindowEvents(void) {
    //while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}
    //while(PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
    //    if (msg.message) {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}
        if(PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void Window::ShowWindow(void) {
    ::ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the instance pointer
    Window* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        // Save the pointer during window creation
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pThis)
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_SIZE:
        width = LOWORD(lParam); height = HIWORD(lParam);
        *fbResize = true;
        break;
    case WM_CLOSE:
        isRunning = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Window::WaitMessages(void) {
    MsgWaitForMultipleObjects(0, nullptr, FALSE, INFINITE, QS_ALLEVENTS);
}

/*inline */void Window::GetWindowArea(int& width, int& height) {
    GetWindowRect(hwnd, &rect);
    width = rect.right - rect.left; height = rect.bottom - rect.top;
}

void Window::SetWindowStyle(WINDOW_STYLE style) {
    LONG currentStyle = GetWindowLong(hwnd, GWL_STYLE);
    DWORD exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    RECT rect;

    // Get the current window rectangle
    GetWindowRect(hwnd, &rect);

    switch (style) {
    case WINDOW_STYLE::BORDERLESS:
        // Remove borders, title bar, etc.
        currentStyle &= ~WS_OVERLAPPEDWINDOW;
        currentStyle |= WS_POPUP;
        exStyle |= WS_EX_TOPMOST;

        // Set the new style
        SetWindowLong(hwnd, GWL_STYLE, currentStyle);
        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

        // Resize window to keep the same dimensions
        SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
        break;

    case WINDOW_STYLE::FULLSCREEN:
        // Remove all borders and title bar, adjust to fullscreen
        currentStyle &= ~WS_OVERLAPPEDWINDOW;
        currentStyle |= WS_POPUP;
        exStyle |= WS_EX_TOPMOST;

        // Get the screen dimensions
        RECT screenRect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

        // Set the new style and resize window
        SetWindowLong(hwnd, GWL_STYLE, currentStyle);
        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
        SetWindowPos(hwnd, nullptr, screenRect.left, screenRect.top, screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
            SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
        break;

    case WINDOW_STYLE::WINDOWED:
        // Restore the default window style (overlapped window with borders and title bar)
        currentStyle |= WS_OVERLAPPEDWINDOW;
        exStyle &= ~WS_EX_TOPMOST;

        // Set the new style
        SetWindowLong(hwnd, GWL_STYLE, currentStyle);
        SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

        // Resize window to original dimensions
        SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
        break;
    }
}

#elif LINUX
// Linux window creation code...
#endif