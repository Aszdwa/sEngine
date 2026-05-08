#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
//
#include "Error/ErrorSystem.h"
#include "Graphics/Color.h" // IWYU pragma: keep
#include "Graphics/Window.h"
#include <iostream>
//
namespace ErrorContext {
enum class IoStream { Out, Err };
} // namespace ErrorContext

namespace ErrorContext {
// Functions:
inline void TerminalLog(const IoStream stream, const Error &err,
                        const Color::AsciiColor color) {
  std::ostream *out = nullptr;

  switch (stream) {
  case IoStream::Out:
    out = &std::cout;
    break;
  case IoStream::Err:
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

inline WindowSystem::DialogResult
DialogLog(const Error &err, const WindowSystem::DialogType type) {
  WindowSystem::DialogLayout dialog_layout;
  //
  std::string title =
      StringSeverity(err.severity) + (std::string) " " + err.source;
  //
  dialog_layout.title = title;
  dialog_layout.type = type;
#ifndef ADDITIONAL_ERROR_INFO
  dialog_layout.message = err.message;
#else
  std::string extended_message = err.message + "\n\n" + "File: " + err.file +
                                 "\n" + "Line: " + std::to_string(err.line) +
                                 "\n" + "Function: " + err.function;
  dialog_layout.message = extended_message;
#endif
  WindowSystem::Dialog dialog(dialog_layout);
  return dialog.Show();
}
}; // namespace ErrorContext

namespace ErrorContext {

inline void Handle(const Error &err) {
  switch (err.severity) {

  case ErrorSeverity::Info: {
    TerminalLog(IoStream::Out, err, Color::AsciiColor(Color::AsciiEnum::Cyan));
    break;
  }

  case ErrorSeverity::Warning: {
    TerminalLog(IoStream::Err, err,
                Color::AsciiColor(Color::AsciiEnum::Yellow));
    break;
  }

  case ErrorSeverity::Error: {
    TerminalLog(IoStream::Err, err, Color::AsciiColor(Color::AsciiEnum::Red));
    DialogLog(err, WindowSystem::DialogType::Ok);
    break;
  }

  case ErrorSeverity::Fatal: {
    TerminalLog(IoStream::Err, err,
                Color::AsciiColor(Color::AsciiEnum::Magenta));
    DialogLog(err, WindowSystem::DialogType::Ok);
    std::abort();
    break;
  }

  default:
    TerminalLog(IoStream::Err, err, Color::AsciiColor(Color::AsciiEnum::White));
    break;
  } // switch case Err
} // ? Handle function
}; // namespace ErrorContext

#endif // ERROR_HANDLER_H