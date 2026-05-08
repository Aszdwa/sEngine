#ifndef ERROR_SYSTEM_H
#define ERROR_SYSTEM_H

#include <string>

#ifdef USE_STD_SOURCE_LOCATION_INSTEAD
#include <source_location>
#endif

namespace ErrorContext {

enum class ErrorSeverity { Info, Warning, Error, Fatal };

struct Error {
public:
  int code = 0;
  std::string message;
  std::string source;
  ErrorSeverity severity = ErrorSeverity::Error;

#ifdef ADDITIONAL_ERROR_INFO
  std::string file;
  int line = 0;
  std::string function;
#endif // ADDITIONAL_ERROR_INFO

public:
  Error(int c, std::string msg, std::string src,
        ErrorSeverity sev = ErrorSeverity::Error)
      : code(c), message(std::move(msg)), source(std::move(src)),
        severity(sev) {}

#ifdef ADDITIONAL_ERROR_INFO

#ifdef USE_STD_SOURCE_LOCATION_INSTEAD

  Error(int c, std::string msg, std::string src, ErrorSeverity sev,
        const std::source_location &loc)
      : code(c), message(std::move(msg)), source(std::move(src)), severity(sev),
        file(loc.file_name()), line(static_cast<int>(loc.line())),
        function(loc.function_name()) {}

#else

  Error(int c, std::string msg, std::string src, ErrorSeverity sev,
        std::string f, int l, std::string func)
      : code(c), message(std::move(msg)), source(std::move(src)), severity(sev),
        file(std::move(f)), line(l), function(std::move(func)) {}

#endif // USE_STD_SOURCE_LOCATION_INSTEAD

#endif // ADDITIONAL_ERROR_INFO
};

#ifdef ADDITIONAL_ERROR_INFO

#ifdef USE_STD_SOURCE_LOCATION_INSTEAD

#define MAKE_ERROR(code, source, severity, msg)                                \
  ErrorContext::Error(code, msg, source, severity,                             \
                      std::source_location::current())

#else

#define MAKE_ERROR(code, source, severity, msg)                                \
  ErrorContext::Error(code, msg, source, severity, FILE_NAME, LINE_NUMBER,     \
                      FUNC_NAME)

#endif

#else

#define MAKE_ERROR(code, source, severity, msg)                                \
  ErrorContext::Error(code, msg, source, severity)

#endif // ADDITIONAL_ERROR_INFO

inline const char *StringSeverity(const ErrorSeverity sev) {
  switch (sev) {
  case ErrorSeverity::Info:
    return "[INFO]";
  case ErrorSeverity::Warning:
    return "[WARNING]";
  case ErrorSeverity::Error:
    return "[ERROR]";
  case ErrorSeverity::Fatal:
    return "[FATAL]";
  default:
    return "[UNKNOWN]";
  }
}

} // namespace ErrorContext

#endif // ERROR_SYSTEM_H