#include "Window.h" // IWYU pragma: keep
//
#ifdef WINDOWS_PLATFORM
#define WIN32_LEAN_AND_MEAN
#include "Windows.h" // IWYU pragma: keep
#endif
//
namespace WindowSystem {
struct Cursor::CursorPlatform {
#ifdef WINDOWS_PLATFORM
  HCURSOR cursor = nullptr;
  bool owned = false;
#endif
};

#ifdef WINDOWS_PLATFORM
// HCURSOR Cursor::nativeHandle() const { return platform->cursor; }
void *Cursor::nativeHandle() const { return platform->cursor; }
#endif

void Cursor::createPlatform() {
  static_assert(sizeof(CursorPlatform) <= sizeof(platformStorage),
                "platformStorage is too small! Increase the size in Window.h");
  static_assert(alignof(CursorPlatform) <= alignof(void *),
                "Alignment mismatch for platformStorage");
  ::new (platformStorage) CursorPlatform();
  platform = getPlatform();
}

Cursor::Cursor() {
  createPlatform();
  Load(Type::ARROW);
}

Cursor::~Cursor() {
  this->Destroy();
  getPlatform()->~CursorPlatform();
}
} // namespace WindowSystem

namespace WindowSystem {
#ifdef WINDOWS_PLATFORM

bool Cursor::Load(const Type &type) {
  LPCSTR winCursor = IDC_ARROW;

  switch (type) {
  case Type::ARROW:
    winCursor = IDC_ARROW;
    break;
  case Type::HAND:
    winCursor = IDC_HAND;
    break;
  case Type::IBEAM:
    winCursor = IDC_IBEAM;
    break;
  case Type::CROSS:
    winCursor = IDC_CROSS;
    break;
  case Type::WAIT:
    winCursor = IDC_WAIT;
    break;
  default:
    return false;
  }

  platform->cursor = LoadCursor(nullptr, winCursor);
  platform->owned = false;
  return platform->cursor != nullptr;
}

bool Cursor::LoadFromFile(const char *path) {
  platform->cursor = static_cast<HCURSOR>(
      LoadImageA(nullptr, path, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE));
  platform->owned = platform->cursor != nullptr;
  return platform->cursor != nullptr;
}

void Cursor::Destroy() {
  if (platform->cursor && platform->owned) {
    DestroyCursor(platform->cursor);
  }

  platform->cursor = nullptr;
  platform->owned = false;
}
#elif LINUX_PLATFORM
// ! Linux placeholder
#endif
}; // namespace WindowSystem