#ifndef PLATFORM_H
#define PLATFORM_H

// ============================================================
// Detect Platform
// ============================================================

// Reset all platform macros (safety)
#if defined(WINDOWS_PLATFORM) || defined(LINUX_PLATFORM) ||                    \
    defined(MACOS_PLATFORM) || defined(ANDROID_PLATFORM) ||                    \
    defined(IOS_PLATFORM) || defined(PLAYSTATION4_PLATFORM) ||                 \
    defined(PLAYSTATION5_PLATFORM) || defined(XBOX_ONE_PLATFORM) ||            \
    defined(XBOX_SERIES_PLATFORM) || defined(NINTENDO_SWITCH_PLATFORM)
#error "Platform macros already defined. Possible mismatch or double inclusion."
#endif

// -------------------- Windows --------------------
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_PLATFORM 1

// -------------------- Linux ----------------------
#elif defined(__linux__)
#define LINUX_PLATFORM 1

// -------------------- macOS ----------------------
#elif defined(__APPLE__) && defined(__MACH__)
#define MACOS_PLATFORM 1

// -------------------- Android --------------------
#elif defined(__ANDROID__)
#define ANDROID_PLATFORM 1

// -------------------- iOS ------------------------
#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define IOS_PLATFORM 1

// -------------------- PlayStation 4 --------------
#elif defined(__ORBIS__)
#define PLAYSTATION4_PLATFORM 1

// -------------------- PlayStation 5 --------------
#elif defined(__PROSPERO__)
#define PLAYSTATION5_PLATFORM 1

// -------------------- Xbox One -------------------
#elif defined(_DURANGO)
#define XBOX_ONE_PLATFORM 1

// -------------------- Xbox Series X|S ------------
#elif defined(_GAMING_XBOX)
#define XBOX_SERIES_PLATFORM 1

// -------------------- Nintendo Switch ------------
#elif defined(__SWITCH__)
#define NINTENDO_SWITCH_PLATFORM 1

// -------------------- Unknown --------------------
#else
#error "Unsupported platform!"
#endif

// ============================================================
// Validation (ensure exactly one platform is defined)
// ============================================================

#if (defined(WINDOWS_PLATFORM) + defined(LINUX_PLATFORM) +                     \
     defined(MACOS_PLATFORM) + defined(ANDROID_PLATFORM) +                     \
     defined(IOS_PLATFORM) + defined(PLAYSTATION4_PLATFORM) +                  \
     defined(PLAYSTATION5_PLATFORM) + defined(XBOX_ONE_PLATFORM) +             \
     defined(XBOX_SERIES_PLATFORM) + defined(NINTENDO_SWITCH_PLATFORM)) != 1

#error "Platform detection failed or multiple platforms defined."
#endif

// ============================================================
// Optional: Platform Name (useful for logging/debug)
// ============================================================

#if defined(WINDOWS_PLATFORM)
#define PLATFORM_NAME "Windows"
#elif defined(LINUX_PLATFORM)
#define PLATFORM_NAME "Linux"
#elif defined(MACOS_PLATFORM)
#define PLATFORM_NAME "macOS"
#elif defined(ANDROID_PLATFORM)
#define PLATFORM_NAME "Android"
#elif defined(IOS_PLATFORM)
#define PLATFORM_NAME "iOS"
#elif defined(PLAYSTATION4_PLATFORM)
#define PLATFORM_NAME "PlayStation 4"
#elif defined(PLAYSTATION5_PLATFORM)
#define PLATFORM_NAME "PlayStation 5"
#elif defined(XBOX_ONE_PLATFORM)
#define PLATFORM_NAME "Xbox One"
#elif defined(XBOX_SERIES_PLATFORM)
#define PLATFORM_NAME "Xbox Series"
#elif defined(NINTENDO_SWITCH_PLATFORM)
#define PLATFORM_NAME "Nintendo Switch"
#endif

#endif // PLATFORM_H