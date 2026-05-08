#ifndef ERROR_FORMATTING_H
#define ERROR_FORMATTING_H

#include "ErrorTypes.h"
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

} // namespace ErrorContext

#endif