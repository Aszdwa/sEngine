#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
//
#include "ErrorTypes.h"
#include "ErrorOutput.h"
//
#ifdef CUSTOM_ERROR_HANDLER
#define REPORT_ERROR(x) Handle(x)
#elif NO_ERROR_HANDLER
#define REPORT_ERROR(x) ((void)0)
#else
inline void PRE_IMPLEMENTED_ERROR_HANDLER(const ErrorContext::Error &error) {
  switch (error.severity) {
  case ErrorContext::ErrorSeverity::Info: {
    ErrorContext::TerminalLog(error, ErrorContext::IoStream::Err);
    break;
  }
  case ErrorContext::ErrorSeverity::Warning: {
    ErrorContext::TerminalLog(error, ErrorContext::IoStream::Err);
    break;
  }
  case ErrorContext::ErrorSeverity::Error: {
    ErrorContext::TerminalLog(error, ErrorContext::IoStream::Err);
    ErrorContext::DialogLog(error, WindowSystem::DialogType::Ok);
    break;
  }
  case ErrorContext::ErrorSeverity::Fatal: {
    ErrorContext::TerminalLog(error, ErrorContext::IoStream::Err);
    ErrorContext::DialogLog(error, WindowSystem::DialogType::Ok);
    std::abort();
    break;
  }
  } // switch severity
}
#define REPORT_ERROR(x) PRE_IMPLEMENTED_ERROR_HANDLER(x)
#endif // CUSTOM_ERROR_HANDLER

#endif // ERROR_HANDLER_H