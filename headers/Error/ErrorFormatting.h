#ifndef ERROR_FORMATTING_H
#define ERROR_FORMATTING_H

#include "ErrorTypes.h"
#include "Graphics/Color.h"
#include <string>

namespace ErrorContext {

[[nodiscard]] inline const char *ToString(const ErrorSeverity severity) {

  switch (severity) {
  case ErrorSeverity::Info:
    return "[INFO]";

  case ErrorSeverity::Warning:
    return "[WARNING]";

  case ErrorSeverity::Error:
    return "[ERROR]";

  case ErrorSeverity::Fatal:
    return "[FATAL]";
  }

  return "[UNKNOWN]";
}

[[nodiscard]] inline std::string BuildMessage(const Error &error) {

  std::string result;

  result += ToString(error.severity);
  result += " Code: ";
  result += std::to_string(error.code);
  result += " | Source: ";
  result += error.source;
  result += " | Message: ";
  result += error.message;

#ifdef ADDITIONAL_ERROR_INFO

  result += "\nFile: ";
  result += error.file;

  result += " | Line: ";
  result += std::to_string(error.line);

  result += " | Function: ";
  result += error.function;

#endif

  return result;
}

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

} // namespace ErrorContext

#endif