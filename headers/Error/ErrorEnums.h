#ifndef ERROR_ENUMS_H
#define ERROR_ENUMS_H
//
namespace ErrorContext {

enum class ErrorSeverity { Info, Warning, Error, Fatal };
enum class IoStream { Out, Err };

}; // namespace ErrorContext

#endif // ERROR_ENUMS_H