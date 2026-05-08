#ifndef ERROR_MACROS_H
#define ERROR_MACROS_H

#include "ErrorTypes.h" // IWYU pragma: keep

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

#endif // ERROR_MACROS_H