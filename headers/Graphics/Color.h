#ifndef COLOR_H
#define COLOR_H
//
namespace Color {
//
struct Rgb_uc;
struct Rgb_f;
//

struct Rgb_uc {
  unsigned char r = 0, g = 0, b = 0;
  // Constructors
  constexpr Rgb_uc() = default;
  constexpr Rgb_uc(unsigned char r, unsigned char g, unsigned char b);
  constexpr Rgb_uc(const Rgb_f &c);
};

struct Rgb_f {
  float r, g, b;
  // Constructors
  constexpr Rgb_f() = default;
  constexpr Rgb_f(float r, float g, float b);
  constexpr Rgb_f(const Rgb_uc &c);
};

enum class AsciiEnum {
  Black = 30,
  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  Cyan = 36,
  White = 37,
  Reset = 0
};

struct AsciiColor {
  AsciiEnum code;
  constexpr const char *to_string() const;
  constexpr AsciiColor();
  constexpr AsciiColor(AsciiEnum color);
};

}; // namespace Color

namespace Color { // Rgb_uc

constexpr Rgb_uc::Rgb_uc(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b) {}

constexpr Rgb_uc::Rgb_uc(const Rgb_f &c)
    : r(static_cast<unsigned char>(c.r * 255.0f)),
      g(static_cast<unsigned char>(c.g * 255.0f)),
      b(static_cast<unsigned char>(c.b * 255.0f)) {}

}; // namespace Color

namespace Color { // Rgb_f

constexpr Rgb_f::Rgb_f(float r, float g, float b) : r(r), g(g), b(b) {}

constexpr Rgb_f::Rgb_f(const Rgb_uc &c)
    : r(c.r / 255.0f), g(c.g / 255.0f), b(c.b / 255.0f) {}

} // namespace Color

namespace Color { // AsciiColor
constexpr AsciiColor::AsciiColor() : code(AsciiEnum::Reset) {}

constexpr AsciiColor::AsciiColor(AsciiEnum color) : code(color) {}

constexpr const char *AsciiColor::to_string() const {
  switch (code) {
  case AsciiEnum::Black:
    return "\x1b[30m";
  case AsciiEnum::Red:
    return "\x1b[31m";
  case AsciiEnum::Green:
    return "\x1b[32m";
  case AsciiEnum::Yellow:
    return "\x1b[33m";
  case AsciiEnum::Blue:
    return "\x1b[34m";
  case AsciiEnum::Magenta:
    return "\x1b[35m";
  case AsciiEnum::Cyan:
    return "\x1b[36m";
  case AsciiEnum::White:
    return "\x1b[37m";
  default:
    return "\x1b[0m";
  }
}

}; // namespace Color

#endif // COLOR_H