#ifndef COMPILER_H
#define COMPILER_H

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else
#define COMPILER_UNKNOWN 1
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
#define DEBUG 1
#else
#define RELEASE 1
#endif

#if defined(COMPILER_MSVC)
#define COMPILER_NAME "MSVC"
#elif defined(COMPILER_CLANG)
#define COMPILER_NAME "Clang"
#elif defined(COMPILER_GCC)
#define COMPILER_NAME "GCC"
#else
#define COMPILER_NAME "Unknown"
#endif

#if defined(DEBUG)
#define BUILD_TYPE "Debug"
#else
#define BUILD_TYPE "Release"
#endif

#define FUNC_NAME __func__

#if COMPILER_MSVC
#define FUNC_SIGNATURE __FUNCSIG__
#elif COMPILER_CLANG || COMPILER_GCC
#define FUNC_SIGNATURE __PRETTY_FUNCTION__
#else
#define FUNC_SIGNATURE FUNC_NAME
#endif

#define FILE_NAME __FILE__
#define LINE_NUMBER __LINE__

#endif // COMPILER_H