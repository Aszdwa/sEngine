#ifndef ERROR_OUTPUT_H
#define ERROR_OUTPUT_H

#include "ErrorEnums.h"
#include "ErrorTypes.h"
#include "ErrorFormatting.h"
#include "Graphics/Color.h"
#include "Graphics/Window.h"

#include <iostream>

namespace ErrorContext {

inline void TerminalLog(const Error &error, IoStream stream,
                        Color::AsciiColor color = Color::AsciiEnum::Reset) {
  std::ostream &output = (stream == IoStream::Err) ? std::cerr : std::cout;
  color = (color.code == Color::AsciiEnum::Reset) ? sevColor(error.severity)
                                                  : color;
  output << color.to_string() << buildMessage(error)
         << Color::AsciiColor().to_string() << std::endl;
}

inline WindowSystem::DialogResult
DialogLog(const Error &error, const WindowSystem::DialogType type) {
  const std::string title =
      std::string(strSeverity(error.severity)) + " " + error.source;
  WindowSystem::DialogLayout layout{title, error.message, type};
  return WindowSystem::Dialog(layout).Show();
}

} // namespace ErrorContext

#endif