#ifndef ERROR_CONFIG_H
#define ERROR_CONFIG_H
//
#include "Compatibility/Compatibility.h" // IWYU pragma: keep
//
#ifdef DEBUG
#define ADDITIONAL_ERROR_INFO
#endif // DEBUG

#ifdef LANG_CPP
#if CPP_VERSION >= 20
#define USE_STD_SOURCE_LOCATION_INSTEAD
#endif // CPP_VERSION
#endif // LANG_CPP

#endif // ERROR_CONFIG_H