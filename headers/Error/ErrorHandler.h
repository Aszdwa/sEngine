#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
//
#include "ErrorSystem.h"
#include "Graphics/Color.h" // IWYU pragma: keep
#include <iostream>
//
#ifdef WINDOWS_PLATFORM
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef OUT
#undef IGNORE
#endif // Platform
//
namespace ErrorContext {
enum class IO_STREAM { OUT, ERR };
enum class DIALOG_TYPE {
  OK,
  HELP,
  CANCEL_CONTINUE,
  ABORT_RETRY_IGNORE,
  OK_CANCEL,
  RETRY_CANCEL,
  YES_NO,
  YES_NO_CANCEL,
};
enum class DIALOG_RESULT {
  OK,
  RETRY,
  CANCEL,
  YES,
  NO,
  IGNORE,
  CONTINUE,
  HELP,
  ABORT,
  UNKNOWN
};
} // namespace ErrorContext

namespace ErrorContext {
// Functions:
inline void TerminalLog(const IO_STREAM stream, const Error err,
                        const Color::AsciiColor color) {
  std::ostream *out = nullptr;

  switch (stream) {
  case IO_STREAM::OUT:
    out = &std::cout;
    break;
  case IO_STREAM::ERR:
    out = &std::cerr;
    break;
  default:
    out = &std::cout;
    break;
  }

  (*out) << color.to_string() << StringSeverity(err.severity)
         << " Code: " << err.code << " | Source: " << err.source
         << " | Message: " << err.message;
#ifdef ADDITIONAL_ERROR_INFO
  (*out) << std::endl;
  (*out) << " File: " << err.file << " | Line: " << err.line
         << " | Function: " << err.function;
#endif
  (*out) << Color::AsciiColor().to_string();
  (*out) << std::endl;
}

inline DIALOG_RESULT DialogLog(Error err, const DIALOG_TYPE type) {
#ifdef WINDOWS_PLATFORM
  UINT flags = MB_ICONERROR;

  // Select button layout
  switch (type) {
  case DIALOG_TYPE::OK:
    flags |= MB_OK;
    break;
  case DIALOG_TYPE::HELP:
    flags |= MB_OK | MB_HELP;
    break;
  case DIALOG_TYPE::CANCEL_CONTINUE:
    flags |= MB_CANCELTRYCONTINUE;
    break;
  case DIALOG_TYPE::ABORT_RETRY_IGNORE:
    flags |= MB_ABORTRETRYIGNORE;
    break;
  case DIALOG_TYPE::OK_CANCEL:
    flags |= MB_OKCANCEL;
    break;
  case DIALOG_TYPE::RETRY_CANCEL:
    flags |= MB_RETRYCANCEL;
    break;
  case DIALOG_TYPE::YES_NO:
    flags |= MB_YESNO;
    break;
  case DIALOG_TYPE::YES_NO_CANCEL:
    flags |= MB_YESNOCANCEL;
    break;
  }

  std::string title =
      StringSeverity(err.severity) + (std::string) " " + err.source;
#ifndef ADDITIONAL_ERROR_INFO
  int result = MessageBoxA(nullptr, err.message.c_str(), title.c_str(), flags);
#else
  std::string extended_message = err.message + "\n\n" + "File: " + err.file +
                                 "\n" + "Line: " + std::to_string(err.line) +
                                 "\n" + "Function: " + err.function;
  int result =
      MessageBoxA(nullptr, extended_message.c_str(), title.c_str(), flags);
#endif

  // Map Win32 result → your enum
  switch (result) {
  case IDOK:
    return DIALOG_RESULT::OK;
  case IDCANCEL:
    return DIALOG_RESULT::CANCEL;
  case IDRETRY:
    return DIALOG_RESULT::RETRY;
  case IDYES:
    return DIALOG_RESULT::YES;
  case IDNO:
    return DIALOG_RESULT::NO;
  case IDIGNORE:
    return DIALOG_RESULT::IGNORE;
  case IDCONTINUE:
    return DIALOG_RESULT::CONTINUE;
  case IDABORT:
    return DIALOG_RESULT::ABORT;
  case IDHELP:
    return DIALOG_RESULT::HELP;
  default:
    return DIALOG_RESULT::UNKNOWN;
  }
#endif
}

} // namespace ErrorContext

#endif // ERROR_HANDLER_H