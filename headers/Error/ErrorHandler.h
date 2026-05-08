#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "ErrorFormatting.h" // IWYU pragma: keep
#include "ErrorOutput.h"

namespace ErrorContext {

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