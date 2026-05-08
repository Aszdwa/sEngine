#include "Window.h" // IWYU pragma: keep
//
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

  hcursor = LoadCursor(nullptr, winCursor);
  return hcursor != nullptr;
}

bool Cursor::LoadFromFile(const char *path) {
  hcursor = static_cast<HCURSOR>(
      LoadImageA(nullptr, path, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE));

  return hcursor != nullptr;
}

void Cursor::Destroy() {
  if (hcursor) {
    DestroyCursor(hcursor); // safe for LoadImage
    hcursor = nullptr;
  }
}
#elif LINUX_PLATFORM
// ! Linux placeholder
#endif
}; // namespace WindowSystem