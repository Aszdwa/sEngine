#include "Graphics/Color.h"  // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
#include "headers/Error/ErrorHandler.h"
using namespace ErrorContext;

#include "Graphics/Window.h"

int main() {
  /*
  Error err =
      MAKE_ERROR(14, "Window", ErrorSeverity::Error, "Couldn't resize window");
  TerminalLog(IO_STREAM::ERR, err, Color::AsciiColor(Color::AsciiEnum::Red));
  */

  /*
  WindowSystem::WindowLayout layout{};
  layout.title = "Janeulas";
  layout.class_name = "Janeulas class";
  layout.cursor = WindowSystem::Cursor::Type::HAND;
  layout.width = 800;
  layout.height = 600;
  layout.posx = 200;
  layout.posy = 200;
  layout.state = WindowSystem::WindowState::NORMAL;
  layout.style = WindowSystem::WindowStyle::NORMAL;
  WindowSystem::Window window(layout);
  if (int r = window.Create(); r != 0) {
    return r;
  }
  window.Show();
  while (window.IsRunning()) {
    window.Update();
  }

  window.Destroy();
  */

  Error err =
      MAKE_ERROR(14, "Window", ErrorSeverity::Error, "Couldn't resize window");
  ErrorContext::Handle(err);

  return 0;
}