#ifndef ERROR_OUTPUT_H
#define ERROR_OUTPUT_H

#include "ErrorFormatting.h"
#include "Graphics/Color.h"
#include "Graphics/Window.h"

#include <iostream>

namespace ErrorContext {

enum class IoStream { Out, Err };

[[nodiscard]] inline std::ostream &SelectStream(const IoStream stream) {

  switch (stream) {
  case IoStream::Out:
    return std::cout;

  case IoStream::Err:
    return std::cerr;
  }

  return std::cout;
}

inline void TerminalLog(const Error &error, const IoStream stream,
                        const Color::AsciiColor &color) {

  auto &output = SelectStream(stream);

  output << color.to_string() << BuildMessage(error)
         << Color::AsciiColor().to_string() << std::endl;
}

inline WindowSystem::DialogResult
DialogLog(const Error &error, const WindowSystem::DialogType type) {

  const std::string title =
      std::string(ToString(error.severity)) + " " + error.source;

  WindowSystem::DialogLayout layout{title, BuildMessage(error), type};

  return WindowSystem::Dialog(layout).Show();
}

} // namespace ErrorContext

#endif