#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "ErrorOutput.h"

namespace ErrorContext {

[[nodiscard]] inline Color::AsciiColor
SeverityColor(const ErrorSeverity severity) {

  switch (severity) {
  case ErrorSeverity::Info:
    return Color::AsciiColor(Color::AsciiEnum::Cyan);

  case ErrorSeverity::Warning:
    return Color::AsciiColor(Color::AsciiEnum::Yellow);

  case ErrorSeverity::Error:
    return Color::AsciiColor(Color::AsciiEnum::Red);

  case ErrorSeverity::Fatal:
    return Color::AsciiColor(Color::AsciiEnum::Magenta);
  }

  return Color::AsciiColor(Color::AsciiEnum::White);
}

[[nodiscard]] inline IoStream SeverityStream(const ErrorSeverity severity) {

  switch (severity) {
  case ErrorSeverity::Info:
    return IoStream::Out;

  case ErrorSeverity::Warning:
  case ErrorSeverity::Error:
  case ErrorSeverity::Fatal:
    return IoStream::Err;
  }

  return IoStream::Err;
}

inline void Handle(const Error &error) {

  TerminalLog(error, SeverityStream(error.severity),
              SeverityColor(error.severity));

  switch (error.severity) {

  case ErrorSeverity::Error:
    DialogLog(error, WindowSystem::DialogType::Ok);
    break;

  case ErrorSeverity::Fatal:
    DialogLog(error, WindowSystem::DialogType::Ok);
    std::abort();
    break;

  default:
    break;
  }
}

} // namespace ErrorContext

#endif // ERROR_HANDLER