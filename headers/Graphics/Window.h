#ifndef WINDOW_H
#define WINDOW_H
//
#include "Compatibility/Compatibility.h" // IWYU pragma: keep
#include <string>
//
#ifdef WINDOWS_PLATFORM
#define WIN32_LEAN_AND_MEAN
#include "Windows.h" // IWYU pragma: keep
#endif
//
namespace WindowSystem {

enum class WindowShow { NORMAL, RESTORE, MINIMIZED, MAXIMIZED, HIDE };

enum class WindowStyle { NORMAL, DIALOG, BORDERLESS, POPUP };

class Cursor {
public:
  enum class Type { ARROW, HAND, IBEAM, CROSS, WAIT, NONE };

public:
  bool Load(const Type &type);
  bool LoadFromFile(const char *path);
  void Destroy();

public:
  Cursor();
  ~Cursor();

public:
  Cursor &operator=(Type type) {
    Destroy();
    Load(type);
    return *this;
  }

  Cursor &operator=(const char *s) {
    Destroy();
    LoadFromFile(s);
    return *this;
  }

private:
  struct CursorPlatform;
  alignas(void *) std::byte platformStorage[16];
  CursorPlatform *platform = nullptr;

public:
  void createPlatform();

  CursorPlatform *getPlatform() {
    return reinterpret_cast<CursorPlatform *>(platformStorage);
  }
  const CursorPlatform *getPlatform() const {
    return reinterpret_cast<const CursorPlatform *>(platformStorage);
  }

#ifdef WINDOWS_PLATFORM
  // HCURSOR nativeHandle() const;
  void *nativeHandle() const;
#endif
}; // class Cursor

struct WindowLayout {
  const char *title = "Window";
  const char *class_name = "DefaultWindowClass";

  unsigned int width = 800;
  unsigned int height = 600;

  int posx = 100;
  int posy = 100;

  WindowShow show = WindowShow::NORMAL;
  WindowStyle style = WindowStyle::NORMAL;
  Cursor cursor;
}; // struct WindowLayout

}; // namespace WindowSystem

namespace WindowSystem {

enum class DialogType {
  Ok,
  Help,
  CancelContinue,
  AbortRetryContinue,
  OkCancel,
  RetryCancel,
  YesNo,
  YesNoCancel,
};
enum class DialogResult {
  Ok,
  Retry,
  Cancel,
  Yes,
  No,
  Ignore,
  Continue,
  Help,
  Abort,
  Unknown
};

struct DialogLayout {
  std::string title;
  std::string message;
  DialogType type = DialogType::Ok;
  //
  DialogLayout() = default;
  DialogLayout(const std::string &title, const std::string &message,
               DialogType type)
      : title(title), message(message), type(type) {}
}; // struct DialogLayout

class Dialog {
private:
  DialogLayout layout;

public:
  Dialog() = default;
  Dialog(const DialogLayout &layout) : layout(layout) {}

public:
  void setLayout(const DialogLayout &param_layout) { layout = param_layout; }

public:
  DialogResult Show() {
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
    int result = MessageBoxA(nullptr, layout.message.c_str(),
                             layout.title.c_str(), flags);
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
    }
  } // switch result
}; // class Dialog
}; // namespace WindowSystem

namespace WindowSystem { // Definitions
class Window {
public:
  int Create();
  void Destroy();
  void Show();
  void Update();
  bool IsRunning() const { return running; }

public:
  Window() = default;
  Window(const WindowLayout &layout);
  ~Window();

private:
  WindowLayout layout{};
  bool running = false;

private:
  struct WindowPlatform;
  alignas(void *) std::byte platformStorage[16];
  WindowPlatform *platform = nullptr;

public:
  WindowPlatform *getPlatform() {
    return reinterpret_cast<WindowPlatform *>(platformStorage);
  }
  const WindowPlatform *getPlatform() const {
    return reinterpret_cast<const WindowPlatform *>(platformStorage);
  }

private:
#ifdef WINDOWS_PLATFORM
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam);
  DWORD GetWindowStyle() const;
  DWORD GetWindowShow() const;
#endif
}; // class Window;

}; // namespace WindowSystem

#endif // WINDOW_H