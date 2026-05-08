#ifndef ERROR_TYPES_H
#define ERROR_TYPES_H
//
#include <string>
#include "ErrorEnums.h"
//
#ifdef USE_STD_SOURCE_LOCATION_INSTEAD
#include <source_location>
#endif

namespace ErrorContext {

struct Error {
  int code = 0;
  std::string message;
  std::string source;
  ErrorSeverity severity = ErrorSeverity::Error;

#ifdef ADDITIONAL_ERROR_INFO
  std::string file;
  int line = 0;
  std::string function;
#endif

  Error() = default;

  Error(int errorCode, std::string errorMessage, std::string errorSource,
        ErrorSeverity errorSeverity = ErrorSeverity::Error)
      : code(errorCode), message(std::move(errorMessage)),
        source(std::move(errorSource)), severity(errorSeverity) {}

#ifdef ADDITIONAL_ERROR_INFO

#ifdef USE_STD_SOURCE_LOCATION_INSTEAD

  Error(int errorCode, std::string errorMessage, std::string errorSource,
        ErrorSeverity errorSeverity, const std::source_location &location)
      : code(errorCode), message(std::move(errorMessage)),
        source(std::move(errorSource)), severity(errorSeverity),
        file(location.file_name()), line(static_cast<int>(location.line())),
        function(location.function_name()) {}

#else // USE_STD_SOURCE_LOCATION_INSTEAD

  Error(int errorCode, std::string errorMessage, std::string errorSource,
        ErrorSeverity errorSeverity, std::string errorFile, int errorLine,
        std::string errorFunction)
      : code(errorCode), message(std::move(errorMessage)),
        source(std::move(errorSource)), severity(errorSeverity),
        file(std::move(errorFile)), line(errorLine),
        function(std::move(errorFunction)) {}

#endif // USE_STD_SOURCE_LOCATION_INSTEAD
#endif // ADDITIONAL_ERROR_INFO
};

} // namespace ErrorContext

#endif // ERROR_TYPES_H