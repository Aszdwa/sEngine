#include "Window.h"

namespace WindowSystem {

Window::Window(const WindowLayout &layout) : layout(layout) {}

#ifdef WINDOWS_PLATFORM

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam) {
  switch (msg) {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

    /*case WM_PAINT:
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_HOTLIGHT + 1));
      EndPaint(hwnd, &ps);
    */
  } // switch (msg)

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD Window::GetWindowStyle() const {
  DWORD style = 0;

  switch (layout.style) {
  case WindowStyle::BORDERLESS:
    style = WS_POPUP;
    break;

  case WindowStyle::POPUP:
    style = WS_POPUPWINDOW;
    break;

  case WindowStyle::NORMAL:
  default:
    style = WS_OVERLAPPEDWINDOW;
    break;
  }

  return style;
}

DWORD Window::GetWindowShow() const {
  switch (layout.show) {
  case WindowShow::HIDE:
    return SW_HIDE;
  case WindowShow::MINIMIZED:
    return SW_MINIMIZE;
  case WindowShow::MAXIMIZED:
    return SW_MAXIMIZE;
  case WindowShow::RESTORE:
    return SW_RESTORE;
  case WindowShow::NORMAL:
  default:
    return SW_SHOWNORMAL;
  }
}

int Window::Create() {
  platform.hInstance = GetModuleHandle(nullptr);

  WNDCLASS wc{};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = platform.hInstance;
  wc.lpszClassName = layout.class_name;
  wc.hCursor = layout.cursor.GetHandle();

  if (!RegisterClass(&wc)) {
    MessageBox(nullptr, "RegisterClass failed", "Error", MB_OK);
    return 1;
  }

  platform.hwnd =
      CreateWindowEx(0, layout.class_name, layout.title, GetWindowStyle(),
                     layout.posx, layout.posy, layout.width, layout.height,
                     nullptr, nullptr, platform.hInstance, nullptr);

  if (!platform.hwnd) {
    MessageBox(nullptr, "CreateWindowEx failed", "Error", MB_OK);
    return 2;
  }

  // ShowWindow(platform.hwnd, SW_HIDE);
  UpdateWindow(platform.hwnd);

  running = true;
  return 0;
}

void Window::Show() {
  ShowWindow(platform.hwnd, GetWindowShow());
  UpdateWindow(platform.hwnd);
}

void Window::Destroy() {
  if (platform.hwnd) {
    DestroyWindow(platform.hwnd);
    platform.hwnd = nullptr;
  }
  running = false;
}

void Window::Update() {
  MSG msg{};
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      running = false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

#endif // WINDOWS_PLATFORM

#ifdef LINUX_PLATFORM
// TODO: X11 / Wayland implementation
#endif // LINUX_PLATFORM

} // namespace WindowSystem