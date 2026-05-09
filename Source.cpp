#include "Graphics/Color.h"  // IWYU pragma: keep
#include "Error/ErrorCore.h" // IWYU pragma: keep
#include "headers/Error/ErrorOutput.h"
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

  Error err{14, "Deu erro oh kkkj", "Window", ErrorSeverity::Warning};
  ErrorContext::TerminalLog(err, IoStream::Err);
  ErrorContext::DialogLog(err, WindowSystem::DialogType::Ok);
  return 0;
}

// ! TODO: Fix error headers include order