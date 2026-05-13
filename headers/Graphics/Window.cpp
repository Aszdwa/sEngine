#include "Graphics/Window.h" // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
//
namespace WindowSystem {
struct Window::WindowPlatform {
#ifdef WINDOWS_PLATFORM
  HWND hwnd = nullptr;
  HINSTANCE hInstance = nullptr;
#endif
};

Window::Window(const WindowLayout &layout) : layout(layout) {
  static_assert(sizeof(WindowPlatform) <= sizeof(platformStorage),
                "platformStorage is too small! Increase the size in Window.h");
  static_assert(alignof(WindowPlatform) <= alignof(void *),
                "Alignment mismatch for platformStorage");
  ::new (platformStorage) WindowPlatform();
  platform = getPlatform();
}

Window::~Window() { getPlatform()->~WindowPlatform(); }

} // namespace WindowSystem

namespace WindowSystem {

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
  platform->hInstance = GetModuleHandle(nullptr);

  WNDCLASS wc{};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = platform->hInstance;
  wc.lpszClassName = layout.class_name;
  wc.hCursor = static_cast<HCURSOR>(layout.cursor.nativeHandle());

  if (!RegisterClass(&wc)) {
    ErrorContext::Error err =
        ErrorContext::MAKE_ERROR(1, "Failed to register window class", "Window",
                                 ErrorContext::ErrorSeverity::Fatal);
    REPORT_ERROR(err);
    return 1;
  }

  platform->hwnd =
      CreateWindowEx(0, layout.class_name, layout.title, GetWindowStyle(),
                     layout.posx, layout.posy, layout.width, layout.height,
                     nullptr, nullptr, platform->hInstance, nullptr);

  if (!platform->hwnd) {
    ErrorContext::Error err =
        ErrorContext::MAKE_ERROR(2, "Failed to create window", "Window",
                                 ErrorContext::ErrorSeverity::Fatal);
    REPORT_ERROR(err);
    return 2;
  }

  // ShowWindow(platform.hwnd, SW_HIDE);
  UpdateWindow(platform->hwnd);

  running = true;
  return 0;
}

void Window::Show() {
  ShowWindow(platform->hwnd, GetWindowShow());
  UpdateWindow(platform->hwnd);
}

void Window::Destroy() {
  if (platform->hwnd) {
    DestroyWindow(platform->hwnd);
    platform->hwnd = nullptr;
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

namespace WindowSystem {
DialogResult Dialog::Show() {
#ifdef WINDOWS_PLATFORM
  UINT flags = MB_ICONERROR;

  switch (layout.type) {
  case DialogType::Ok:
    flags |= MB_OK;
    break;
  case DialogType::Help:
    flags |= MB_OK | MB_HELP;
    break;
  case DialogType::CancelContinue:
    flags |= MB_CANCELTRYCONTINUE;
    break;
  case DialogType::AbortRetryContinue:
    flags |= MB_ABORTRETRYIGNORE;
    break;
  case DialogType::OkCancel:
    flags |= MB_OKCANCEL;
    break;
  case DialogType::RetryCancel:
    flags |= MB_RETRYCANCEL;
    break;
  case DialogType::YesNo:
    flags |= MB_YESNO;
    break;
  case DialogType::YesNoCancel:
    flags |= MB_YESNOCANCEL;
    break;
  }
  int result =
      MessageBoxA(nullptr, layout.message.c_str(), layout.title.c_str(), flags);
#endif // WINDOWS_PLATFORM
  switch (result) {
  case IDOK:
    return DialogResult::Ok;
  case IDCANCEL:
    return DialogResult::Cancel;
  case IDRETRY:
    return DialogResult::Retry;
  case IDYES:
    return DialogResult::Yes;
  case IDNO:
    return DialogResult::No;
  case IDIGNORE:
    return DialogResult::Ignore;
  case IDCONTINUE:
    return DialogResult::Continue;
  case IDABORT:
    return DialogResult::Abort;
  case IDHELP:
    return DialogResult::Help;
  default:
    return DialogResult::Unknown;
  } // switch result
}
}; // namespace WindowSystem