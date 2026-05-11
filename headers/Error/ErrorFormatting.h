#ifndef ERROR_FORMATTING_H
#define ERROR_FORMATTING_H
//
#include "Compatibility/Compatibility.h" // IWYU pragma: keep
#include "Error/ErrorTypes.h"
#include "Error/ErrorEnums.h"
#include "Graphics/Color.h"
#include <string>
//
namespace ErrorContext {

inline std::string strSeverity(const ErrorSeverity &sev) {
  switch (sev) {
  case ErrorSeverity::Info:
    return "[INFO]";
  case ErrorSeverity::Warning:
    return "[WARNING]";
  case ErrorSeverity::Error:
    return "[ERROR]";
  case ErrorSeverity::Fatal:
    return "[FATAL]";
  } // switch severity
  return "[UNKNOWN]";
}

inline Color::AsciiEnum sevColor(const ErrorSeverity &sev) {
  switch (sev) {
  case ErrorSeverity::Info:
    return Color::AsciiEnum::Blue;
  case ErrorSeverity::Warning:
    return Color::AsciiEnum::Yellow;
  case ErrorSeverity::Error:
    return Color::AsciiEnum::Red;
  case ErrorSeverity::Fatal:
    return Color::AsciiEnum::Magenta;
  } // switch severity
  return Color::AsciiEnum::Reset;
}

inline std::string buildMessage(const Error &err) {
  std::string message;
  message += strSeverity(err.severity);
  message += " : " + err.message;
#ifdef ADDITIONAL_ERROR_INFO
  message += "\n" + err.file + " at line: " + std::to_string(err.line);
  message += "\n" + err.function;
#endif

  return message;
}

}; // namespace ErrorContext

namespace ErrorContext { // MACROS
#ifdef DEBUG
#define MAKE_ERROR(code, msg, src, sev)                                        \
  Error(code, msg, src, sev, FILE_NAME, LINE_NUMBER, FUNC_NAME)
#else
#define MAKE_ERROR(code, msg, src, sev) Error(code, msg, src, sev)
#endif

}; // namespace ErrorContext

#endif // ERROR_FORMATING_H