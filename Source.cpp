#include "Graphics/Color.h" // IWYU pragma: keep
#define CUSTOM_ERROR_HANDLER
#include "Error/ErrorCore.h" // IWYU pragma: keep
using namespace ErrorContext;

#include "Graphics/Window.h"

void Handle(Error &error) { TerminalLog(error, IoStream::Err); }

int main() {
  WindowSystem::WindowLayout layout{};
  layout.title = "Janeulas";
  layout.class_name = "Janeulas class";
  layout.cursor = WindowSystem::Cursor::Type::HAND;
  layout.width = 800;
  layout.height = 600;
  layout.posx = 9;
  layout.posy = 0;
  layout.style = WindowSystem::WindowStyle::NORMAL;
  layout.show = WindowSystem::WindowShow::NORMAL;
  WindowSystem::Window window(layout);

  if (int r = window.Create(); r != 0) {
    return r;
  }
  window.Show();
  while (window.IsRunning()) {
    window.Update();
  }

  window.Destroy();
  return 0;
}

// ! TODO: Fix header leak