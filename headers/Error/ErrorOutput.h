#ifndef ERROR_OUTPUT_H
#define ERROR_OUTPUT_H

#include "ErrorEnums.h"
#include "ErrorTypes.h"
#include "ErrorFormatting.h"
#include "Graphics/Color.h"
#include "Graphics/Window/Window.h"

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
  WindowSystem::DialogLayout layout{error.source, buildMessage(error), type};
  return WindowSystem::Dialog(layout).Show();
}

} // namespace ErrorContext

#endif