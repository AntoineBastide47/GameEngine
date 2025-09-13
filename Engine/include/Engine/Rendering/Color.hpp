//
// Color.hpp
// Author: Antoine Bastide
// Date: 06.09.2025
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <format>

#include "Engine/Reflection/Concepts.hpp"
#include "Engine/Reflection/ICustomEditor.hpp"
#include "Engine/Reflection/Reflectable.hpp"
#include "Color.gen.hpp"

namespace Engine::Rendering {
  class Color final : public Reflection::Reflectable, public Reflection::ICustomEditor {
    SERIALIZE_COLOR
      static constexpr auto constexprFmod = [](const float x, const float y) constexpr -> float {
        return x - static_cast<int>(x / y) * y;
      };

      static constexpr auto constexprAbs = [](const float x) constexpr -> float {
        return x < 0.0f ? -x : x;
      };

      static constexpr float inv255 = 1.0f / 255.0f;
    public:
      float r, g, b, a;

      constexpr Color()
        : r(0), g(0), b(0), a(0) {}

      constexpr Color(const float r, const float g, const float b, const float a)
        : r(r), g(g), b(b), a(a) {}

      explicit constexpr Color(const float color[4])
        : r(color[0]), g(color[1]), b(color[2]), a(color[3]) {}

      /// @returns A new Color resulting from the component-wise addition of this color and the given color
      constexpr Color operator+(const Color &other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
      }

      /// @returns A new Color resulting from the component-wise substraction of this color and the given color
      constexpr Color operator-(const Color &other) const {
        return Color(r - other.r, g - other.g, b - other.b, a - other.a);
      }

      /// @returns A new Color resulting from the component-wise multiplication of this color and the given color
      constexpr Color operator*(const Color &other) const {
        return Color(r * other.r, g * other.g, b * other.b, a * other.a);
      }

      template<Reflection::IsNumber T>
      /// @returns A new Color resulting from the component-wise multiplication of this color and the given scalar
      constexpr Color operator*(const T scalar) const {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
      }

      template<Reflection::IsNumber T>
      /// @returns A new Color resulting from the component-wise multiplication of this color and the given scalar
      constexpr friend Color operator*(const T scalar, const Color &color) {
        return color * scalar;
      }

      template<Reflection::IsNumber T>
      /// @returns A new Color resulting from the component-wise division of this color and the given scalar
      constexpr Color operator/(const T scalar) const {
        const float s = 1.0f / static_cast<float>(scalar);
        return Color(r * s, g * s, b * s, a * s);
      }

      /// @returns this Color after component-wise addition with the given color
      constexpr Color &operator+=(const Color &other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
      }

      /// @returns this Color after component-wise substraction with the given color
      constexpr Color &operator-=(const Color &other) {
        r -= other.r;
        g -= other.g;
        b -= other.b;
        a -= other.a;
        return *this;
      }

      /// @returns this Color after component-wise multiplication with the given color
      constexpr Color &operator*=(const Color &other) {
        r *= other.r;
        g *= other.g;
        b *= other.b;
        a *= other.a;
        return *this;
      }

      template<Reflection::IsNumber T>
      /// @returns this Color after component-wise multiplication with the given scalar
      constexpr Color &operator*=(const T scalar) {
        const float s = static_cast<float>(scalar);
        r *= s;
        g *= s;
        b *= s;
        a *= s;
        return *this;
      }

      template<Reflection::IsNumber T>
      /// @returns this Color after component-wise division with the given scalar
      constexpr Color &operator/=(const T scalar) {
        const float s = 1.0f / static_cast<float>(scalar);
        return *this *= s;
      }

      /// @returns True if this Color is equal to the given one, False if not
      constexpr bool operator==(const Color &other) const {
        return constexprAbs(r - other.r) < 0.001f &&
               constexprAbs(g - other.g) < 0.001f &&
               constexprAbs(b - other.b) < 0.001f &&
               constexprAbs(a - other.a) < 0.001f;
      }

      /// @returns True if this Color is not equal to the given one, False if not
      constexpr bool operator!=(const Color &other) const {
        return !(*this == other);
      }

      /// @returns A reference to the color component at the given index (0 = r, 1 = g, 2 = b, 3 = a)
      constexpr float &operator[](const size_t index) {
        switch (index) {
          case 0:
            return r;
          case 1:
            return g;
          case 2:
            return b;
          case 3:
            return a;
          default:
            throw std::out_of_range("Color index out of range");
        }
      }

      /// @returns A constant reference to the color component at the given index (0 = r, 1 = g, 2 = b, 3 = a)
      constexpr const float &operator[](const size_t index) const {
        switch (index) {
          case 0:
            return r;
          case 1:
            return g;
          case 2:
            return b;
          case 3:
            return a;
          default:
            throw std::out_of_range("Color index out of range");
        }
      }

      friend std::ostream &operator<<(std::ostream &os, const Color &c) {
        return os << std::format("Color({:.3f}, {:.3f}, {:.3f}, {:.3f})", c.r, c.g, c.b, c.a);
      }

      /// Converts a color from HSV (hue, saturation, value) space to an RGBA Color
      /// @param h Hue in degrees in the range [0, 360)
      /// @param s Saturation in the range [0, 1]
      /// @param v Value in the range [0, 1]
      /// @param a Alpha (optional, defaults to 1.0f)
      /// @returns A Color in RGBA representation corresponding to the given HSV values.
      static constexpr Color FromHSV(const float h, const float s, const float v, const float a = 1.0f) {
        float H = constexprFmod(h, 360.0f);
        if (H < 0.0f)
          H += 360.0f;

        const float c = v * s;
        const float x = c * (1.0f - constexprAbs(constexprFmod(H / 60.0f, 2.0f) - 1.0f));
        const float m = v - c;

        float r, g, b;
        if (H < 60.0f) {
          r = c;
          g = x;
          b = 0.0f;
        } else if (H < 120.0f) {
          r = x;
          g = c;
          b = 0.0f;
        } else if (H < 180.0f) {
          r = 0.0f;
          g = c;
          b = x;
        } else if (H < 240.0f) {
          r = 0.0f;
          g = x;
          b = c;
        } else if (H < 300.0f) {
          r = x;
          g = 0.0f;
          b = c;
        } else {
          r = c;
          g = 0.0f;
          b = x;
        }

        return Color(r + m, g + m, b + m, a);
      }

      /// Converts an RGB Color to HSV (hue, saturation, value) representation
      /// @param color The color to convert to HSV
      /// @param h The resulting hue in degrees in the range [0, 360)
      /// @param s The resulting saturation in the range [0, 1]
      /// @param v The resulting value in the range [0, 1]
      static constexpr void RGBToHSV(const Color &color, float &h, float &s, float &v) {
        const auto c = color.Clamped();
        const float max = std::max({c.r, c.g, c.b});
        const float min = std::min({c.r, c.g, c.b});
        const float delta = max - min;

        v = max;
        s = max > 0.0f ? delta / max : 0.0f;

        if (delta > 0.0f) {
          if (max == c.r)
            h = 60.0f * constexprFmod((c.g - c.b) / delta, 6.0f);
          else if (max == c.g)
            h = 60.0f * ((c.b - c.r) / delta + 2.0f);
          else
            h = 60.0f * ((c.r - c.g) / delta + 4.0f);

          if (h < 0)
            h += 360.0f;
        } else
          h = 0.0f;
      }

      /// Constructs a Color from a hexadecimal string representation.
      /// The string may be in the form "#RRGGBB" or "#RRGGBBAA", with or without a leading '#'.
      /// Each pair of hex digits corresponds to one channel in the range [0, 255], which is normalized to [0, 1].
      /// @param hex The hex color representation to convert
      /// @returns A Color corresponding to the given hexadecimal code
      static constexpr Color FromHex(const std::string_view &hex) {
        if (hex.empty())
          return Black();

        const size_t start = hex[0] == '#' ? 1 : 0;
        if (hex.size() - start < 6)
          return Black();

        constexpr auto hexToInt = [](const char c) constexpr -> int {
          if (c >= '0' && c <= '9')
            return c - '0';
          if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
          if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
          return 0;
        };

        const int r = hexToInt(hex[start]) * 16 + hexToInt(hex[start + 1]);
        const int g = hexToInt(hex[start + 2]) * 16 + hexToInt(hex[start + 3]);
        const int b = hexToInt(hex[start + 4]) * 16 + hexToInt(hex[start + 5]);
        float a = 1.0f;

        if (hex.size() - start >= 8) {
          const int alpha = hexToInt(hex[start + 6]) * 16 + hexToInt(hex[start + 7]);
          a = alpha * inv255;
        }

        return Color(r * inv255, g * inv255, b * inv255, a);
      }

      /// Converts this Color to a hexadecimal string representation.
      /// The result is in the form "#RRGGBB" by default, or "#RRGGBBAA" if `includeAlpha` is true.
      /// Each channel is clamped to [0, 1], scaled to [0, 255], and encoded as two hexadecimal digits.
      /// @returns A string containing the hexadecimal representation of the color
      [[nodiscard]] std::string ToHex(bool includeAlpha = false) const;

      /// Converts this Color from gamma space to linear space by applying gamma correction
      /// to the RGB components while leaving the alpha component unchanged
      /// @returns A new Color in linear space
      [[nodiscard]] constexpr Color ToLinear() const {
        auto toLinear = [](const float channel) constexpr {
          if (channel <= 0.04045f)
            return channel / 12.92f;

          return std::pow((channel + 0.055f) / 1.055f, 2.4f);
        };
        return Color(toLinear(r), toLinear(g), toLinear(b), a);
      }

      /// Converts this Color from linear space to sRGB space by applying the inverse gamma correction
      /// to the RGB components while leaving the alpha component unchanged
      /// @returns A new Color in sRGB space
      [[nodiscard]] constexpr Color ToSRGB() const {
        auto toSRGB = [](const float channel) constexpr {
          if (channel <= 0.0031308f)
            return channel * 12.92f;

          return 1.055f * std::pow(channel, 1.0f / 2.4f) - 0.055f;
        };
        return Color(toSRGB(r), toSRGB(g), toSRGB(b), a);
      }

      /// Computes the relative luminance of this Color using a weighted sum of the RGB components,
      /// typically following the Rec. 709 standard (0.2126 * R + 0.7152 * G + 0.0722 * B).
      /// The alpha component is ignored.
      /// @returns The luminance value in the range [0, 1]
      [[nodiscard]] constexpr float Luminance() const {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
      }

      /// Clamps all components of this Color to the range [0, 1].
      /// Values below 0 are set to 0, and values above 1 are set to 1.
      /// @returns A new Color with all components clamped.
      [[nodiscard]] constexpr Color Clamped() const {
        return Color(
          std::clamp(r, 0.0f, 1.0f),
          std::clamp(g, 0.0f, 1.0f),
          std::clamp(b, 0.0f, 1.0f),
          std::clamp(a, 0.0f, 1.0f)
        );
      }

      /// Clamps all components of this Color to the range [0, 1] in-place.
      /// Values below 0 are set to 0, and values above 1 are set to 1.
      constexpr void Clamp() {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
        a = std::clamp(a, 0.0f, 1.0f);
      }

      /// @returns A copy of this Color with its alpha component replaced by the given alpha value
      [[nodiscard]] constexpr Color WithAlpha(const float newAlpha) const {
        return Color(r, g, b, newAlpha);
      }

      /// Packs this Color into a 32-bit unsigned integer in RGBA order.
      /// Each component is clamped to [0, 1], scaled to [0, 255], and stored as one byte.
      /// @returns A uint32_t representing the color in 0xRRGGBBAA format.
      [[nodiscard]] constexpr uint32_t ToRGBA32() const {
        const auto c = Clamped();
        const uint32_t ir = static_cast<uint32_t>(c.r * 255.0f + 0.5f);
        const uint32_t ig = static_cast<uint32_t>(c.g * 255.0f + 0.5f);
        const uint32_t ib = static_cast<uint32_t>(c.b * 255.0f + 0.5f);
        const uint32_t ia = static_cast<uint32_t>(c.a * 255.0f + 0.5f);
        return ir << 24 | ig << 16 | ib << 8 | ia;
      }

      /// Constructs a Color from a 32-bit unsigned integer in RGBA order.
      /// Each byte is interpreted as one component in [0, 255] and normalized to [0, 1].
      /// @param rgba The 32-bit integer containing RGBA components.
      /// @returns A Color corresponding to the given RGBA value.
      static constexpr Color FromRGBA32(const uint32_t rgba) {
        const float r = (rgba >> 24 & 0xFF) * inv255;
        const float g = (rgba >> 16 & 0xFF) * inv255;
        const float b = (rgba >> 8 & 0xFF) * inv255;
        const float a = (rgba & 0xFF) * inv255;
        return Color(r, g, b, a);
      }

      /// Linearly interpolates between two Colors `a` and `b` by a factor `t`.
      /// The interpolation is performed component-wise on RGBA channels.
      /// @param a The first color to interpolate
      /// @param b The second color to interpolate
      /// @param t Interpolation factor in [0, 1], where 0 returns `a` and 1 returns `b`.
      /// @returns The interpolated Color.
      static constexpr Color Lerp(const Color &a, const Color &b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
      }

      /// @returns A Color representing pure red (R=1, G=0, B=0, A=1)
      static constexpr Color Red() {
        return Color(1.0f, 0.0f, 0.0f, 1.0f);
      }

      /// @returns A Color representing pure green (R=0, G=1, B=0, A=1)
      static constexpr Color Green() {
        return Color(0.0f, 1.0f, 0.0f, 1.0f);
      }

      /// @returns A Color representing pure blue (R=0, G=0, B=1, A=1)
      static constexpr Color Blue() {
        return Color(0.0f, 0.0f, 1.0f, 1.0f);
      }

      /// @returns A Color representing pure white (R=1, G=1, B=1, A=1)
      static constexpr Color White() {
        return Color(1.0f, 1.0f, 1.0f, 1.0f);
      }

      /// @returns A Color representing pure black (R=0, G=0, B=0, A=1)
      static constexpr Color Black() {
        return Color(0.0f, 0.0f, 0.0f, 1.0f);
      }

      /// @returns A Color representing yellow (R=1, G=0.92, B=0.016, A=1)
      static constexpr Color Yellow() {
        return Color(1.0f, 0.92f, 0.016f, 1.0f);
      }

      /// @returns A Color representing cyan (R=0, G=1, B=1, A=1)
      static constexpr Color Cyan() {
        return Color(0.0f, 1.0f, 1.0f, 1.0f);
      }

      /// @returns A Color representing magenta (R=1, G=0, B=1, A=1)
      static constexpr Color Magenta() {
        return Color(1.0f, 0.0f, 1.0f, 1.0f);
      }

      /// @returns A Color representing gray (R=0.5, G=0.5, B=0.5, A=1)
      static constexpr Color Gray() {
        return Color(0.5f, 0.5f, 0.5f, 1.0f);
      }

      /// @returns A Color representing grey (alias for Gray)
      static constexpr Color Grey() {
        return Gray();
      }

      /// @returns A fully transparent Color (R=0, G=0, B=0, A=0)
      static constexpr Color Clear() {
        return Color(0.0f, 0.0f, 0.0f, 0.0f);
      }

      #if ENGINE_EDITOR
      bool OnRenderInEditor(const std::string &name, bool isConst, bool readOnly) override;
      #endif
  };
}

#endif //COLOR_HPP
