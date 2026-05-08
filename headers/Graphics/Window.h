#ifndef WINDOW_H
#define WINDOW_H
//
#include "Compatibility/Compatibility.h" // IWYU pragma: keep
//
#ifdef WINDOWS_PLATFORM
#define WIN32_LEAN_AND_MEAN
#include "Windows.h" // IWYU pragma: keep
#endif
//
namespace WindowSystem {
enum class WindowState {
  MINIMIZED,
  FULLSCREEN,
  MAXIMIZED,
  HIDDEN,
  NORMAL
}; // enum class WindowState

enum class WindowStyle { NORMAL, DIALOG, BORDERLESS, POPUP };

class Cursor {
public:
  enum class Type { ARROW, HAND, IBEAM, CROSS, WAIT, NONE };

public:
  bool Load(const Type &type);
  bool LoadFromFile(const char *path);
  void Destroy();

public:
  Cursor() { Load(Type::ARROW); };
  Cursor(Type type) { Load(type); }
  Cursor(const char *path) { LoadFromFile(path); }

public:
#ifdef WINDOWS_PLATFORM
  HCURSOR GetHandle() const { return hcursor; }
#endif

private:
#ifdef WINDOWS_PLATFORM
  HCURSOR hcursor;
#endif // WINDOWS_PLATFORM
}; // class Cursor

struct WindowLayout {
  const char *title = "Window";
  const char *class_name = "DefaultWindowClass";

  unsigned int width = 800;
  unsigned int height = 600;

  int posx = 100;
  int posy = 100;

  WindowState state = WindowState::NORMAL;
  WindowStyle style = WindowStyle::NORMAL;
  Cursor cursor;
}; // struct WindowLayout

struct WindowPlatform {
#ifdef WINDOWS_PLATFORM
  HWND hwnd = nullptr;
  HINSTANCE hInstance = nullptr;
#endif
};

}; // namespace WindowSystem
//
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

private:
  WindowLayout layout{};
  WindowPlatform platform{};
  bool running = false;

private:
#ifdef WINDOWS_PLATFORM
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam);
  DWORD GetWindowStyle() const;
#endif
}; // class Window;

}; // namespace WindowSystem

namespace WindowSystem { // Implementations
#ifdef WINDOWS_PLATFORM

#elif LINUX_PLATFORM

#endif // PLATFORM SPECIFIC CHECK
}; // namespace WindowSystem

#endif // WINDOW_H