#include "Error/ErrorEnums.h"
#include "Graphics/Color.h"  // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
using namespace ErrorContext;

#include "Graphics/Window.h"

int main() {
  WindowSystem::WindowLayout layout{};
  layout.title = "Janeulas";
  layout.class_name = "Janeulas class";
  layout.cursor = WindowSystem::Cursor::Type::ARROW;
  layout.width = 800;
  layout.height = 600;
  layout.posx = 200;
  layout.posy = 200;
  layout.style = WindowSystem::WindowStyle::POPUP;
  layout.show = WindowSystem::WindowShow::MAXIMIZED;
  WindowSystem::Window window(layout);

  if (int r = window.Create(); r != 0) {
    return r;
  }
  window.Show();
  while (window.IsRunning()) {
    window.Update();
  }

  /*
  Error err = MAKE_ERROR(14, "Hello, world!", "Window", ErrorSeverity::Error);
  TerminalLog(err, IoStream::Err);
  */

  window.Destroy();
  return 0;
}

// ! TODO: Fix error headers include order