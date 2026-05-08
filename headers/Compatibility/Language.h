#ifndef LANGUAGE_H
#define LANGUAGE_H
#pragma once

// ======================================================
// Language Detection
// ======================================================

#if defined(__cplusplus)
#define LANG_CPP 1
#define LANG_C 0
#else
#define LANG_CPP 0
#define LANG_C 1
#endif

// ======================================================
// Default Versions (0 = not applicable / unknown)
// ======================================================

#define CPP_VERSION 0
#define C_VERSION 0

// ======================================================
// C++ Version Detection
// ======================================================

#if LANG_CPP

// Raw version detection
#if defined(_MSC_VER) && !defined(__clang__)
// MSVC (needs /Zc:__cplusplus for correctness, fallback handled)
#define CPP_RAW_VERSION _MSVC_LANG
#else
#define CPP_RAW_VERSION __cplusplus
#endif

// Normalize to simple integer version
#undef CPP_VERSION

#if CPP_RAW_VERSION >= 202302L
#define CPP_VERSION 23
#elif CPP_RAW_VERSION >= 202002L
#define CPP_VERSION 20
#elif CPP_RAW_VERSION >= 201703L
#define CPP_VERSION 17
#elif CPP_RAW_VERSION >= 201402L
#define CPP_VERSION 14
#elif CPP_RAW_VERSION >= 201103L
#define CPP_VERSION 11
#else
#define CPP_VERSION 98
#endif

#endif // LANG_CPP

// ======================================================
// C Version Detection
// ======================================================

#if LANG_C

// Raw version detection
#ifdef __STDC_VERSION__
#define C_RAW_VERSION __STDC_VERSION__
#else
#define C_RAW_VERSION 0L
#endif

// Normalize to simple integer version
#undef C_VERSION

#if C_RAW_VERSION >= 201710L
#define C_VERSION 17
#elif C_RAW_VERSION >= 201112L
#define C_VERSION 11
#elif C_RAW_VERSION >= 199901L
#define C_VERSION 99
#else
#define C_VERSION 90
#endif

#endif // LANG_C

// ======================================================
// Language Name (optional helper)
// ======================================================

#if LANG_CPP
#if CPP_VERSION == 23
#define LANG_NAME "C++23"
#elif CPP_VERSION == 20
#define LANG_NAME "C++20"
#elif CPP_VERSION == 17
#define LANG_NAME "C++17"
#elif CPP_VERSION == 14
#define LANG_NAME "C++14"
#elif CPP_VERSION == 11
#define LANG_NAME "C++11"
#else
#define LANG_NAME "C++98"
#endif

#elif LANG_C
#if C_VERSION == 17
#define LANG_NAME "C17"
#elif C_VERSION == 11
#define LANG_NAME "C11"
#elif C_VERSION == 99
#define LANG_NAME "C99"
#else
#define LANG_NAME "C90"
#endif

#else
#define LANG_NAME "Unknown"
#endif

// ======================================================
// Sanity Check (optional, but recommended)
// ======================================================

#if LANG_CPP && LANG_C
#error "Invalid state: both C and C++ detected"
#endif

#endif // LANGUAGE_H