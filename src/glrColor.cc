#include "glrColor.hh"

#include <commons/misc.hh>
#include <numeric>

namespace glr
{
  constexpr uint8_t s_ui8Max = std::numeric_limits<uint8_t>::max();
  constexpr uint16_t s_ui16Max = std::numeric_limits<uint16_t>::max();
  
  Color Color::operator +(Color const &other) const
  {
    Color out;
    out.red = this->red + other.red;
    out.green = this->green + other.green;
    out.blue = this->blue + other.blue;
    out.alpha = this->alpha + other.alpha;
    return out;
  }
  
  bool Color::operator ==(Color const &other) const
  {
    return this->red == other.red && this->green == other.green && this->blue == other.blue && this->alpha == other.alpha;
  }
  
  void Color::fromRGBf(float r, float g, float b)
  {
    this->red = (ColorFmt) (bound<float>(r, 0.0f, 1.0f) * s_ui16Max);
    this->green = (ColorFmt) (bound<float>(g, 0.0f, 1.0f) * s_ui16Max);
    this->blue = (ColorFmt) (bound<float>(b, 0.0f, 1.0f) * s_ui16Max);
  }
  
  void Color::fromRGBAf(float r, float g, float b, float a)
  {
    this->red = (ColorFmt) (bound<float>(r, 0.0f, 1.0f) * s_ui16Max);
    this->green = (ColorFmt) (bound<float>(g, 0.0f, 1.0f) * s_ui16Max);
    this->blue = (ColorFmt) (bound<float>(b, 0.0f, 1.0f) * s_ui16Max);
    this->alpha = (ColorFmt) (bound<float>(a, 0.0f, 1.0f) * s_ui16Max);
  }
  
  void Color::fromRGBui8(uint8_t r, uint8_t g, uint8_t b)
  {
    this->red = (ColorFmt) (r * 256);
    this->green = (ColorFmt) (g * 256);
    this->blue = (ColorFmt) (b * 256);
  }
  
  void Color::fromRGBAui8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
    this->red = (ColorFmt) (r * 256);
    this->green = (ColorFmt) (g * 256);
    this->blue = (ColorFmt) (b * 256);
    this->alpha = (ColorFmt) (a * 256);
  }
  
  void Color::fromRGBui16(uint16_t r, uint16_t g, uint16_t b)
  {
    this->red = r;
    this->green = g;
    this->blue = b;
  }
  
  void Color::fromRGBAui16(uint16_t r, uint16_t g, uint16_t b, uint16_t a)
  {
    this->red = r;
    this->green = g;
    this->blue = b;
    this->alpha = a;
  }
  
  void Color::fromHex(uint32_t hex)
  {
    this->alpha = (ColorFmt) (((hex & 0xFF000000) >> 24) * 256);
    this->red = (ColorFmt) (((hex & 0x00FF0000) >> 16) * 256);
    this->green = (ColorFmt) (((hex & 0x0000FF00) >> 8) * 256);
    this->blue = (ColorFmt) ((hex & 0x000000FF) * 256);
  }
  
  void Color::fromWeb(std::string const &color)
  {
    if(color.empty() || color[0] != '#')
    {
      printf("Color::fromWeb: Invalid color formatting.\n");
      return;
    }
    uint32_t conv = (uint32_t) (std::strtol(color.data() + 1, nullptr, 16));
    switch(color.size())
    {
      default:
        printf("Color::fromWeb: Invalid color formatting.\n");
        break;
      case 4:
        this->blue = (ColorFmt) ((conv & 0x00F) * 256);
        this->green = (ColorFmt) ((conv & 0x0F0 << 4) * 256);
        this->red = (ColorFmt) ((conv & 0xF00 << 8) * 256);
        break;
      case 5:
        this->alpha = (ColorFmt) ((conv & 0x000F) * 256);
        this->blue = (ColorFmt) ((conv & 0x00F0 << 4) * 256);
        this->green = (ColorFmt) ((conv & 0x0F00 << 8) * 256);
        this->red = (ColorFmt) ((conv & 0xF000 << 12) * 256);
        break;
      case 7:
        this->red = (ColorFmt) ((conv & 0xFF000000 << 16) * 256);
        this->green = (ColorFmt) ((conv & 0xFF000000 << 8) * 256);
        this->blue = (ColorFmt) ((conv & 0xFF000000) * 256);
        break;
      case 9:
        this->red = (ColorFmt) ((conv & 0xFF000000 << 24) * 256);
        this->green = (ColorFmt) ((conv & 0xFF000000 << 16) * 256);
        this->blue = (ColorFmt) ((conv & 0xFF000000 << 8) * 256);
        this->alpha = (ColorFmt) ((conv & 0xFF000000) * 256);
        break;
    }
  }
  
  vec3<float> Color::asRGBf() const
  {
    return {(float) this->red / s_ui16Max, (float) this->green / s_ui16Max, (float) this->blue / s_ui16Max};
  }
  
  vec4<float> Color::asRGBAf() const
  {
    return {(float) this->red / s_ui16Max, (float) this->green / s_ui16Max, (float) this->blue / s_ui16Max, (float) this->alpha / s_ui16Max};
  }
  
  vec3<uint8_t> Color::asRGBui8() const
  {
    return {(uint8_t) (this->red / 256), (uint8_t) (this->green / 256), (uint8_t) (this->blue / 256)};
  }
  
  vec4<uint8_t> Color::asRGBAui8() const
  {
    return {(uint8_t) (this->red / 256), (uint8_t) (this->green / 256), (uint8_t) (this->blue / 256), (uint8_t) (this->alpha / 256)};
  }
  
  vec3<uint16_t> Color::asRGBui16() const
  {
    return {this->red, this->green, this->blue};
  }
  
  vec4<uint16_t> Color::asRGBAui16() const
  {
    return {this->red, this->green, this->blue, this->alpha};
  }
  
  uint32_t Color::asHex() const
  {
    return (uint8_t) (this->alpha / 256) << 24 | (uint8_t) (this->red / 256) << 16 | (uint8_t) (this->green / 256) << 8 | (uint8_t) (this->blue / 256);
  }
  
  std::string Color::asWeb() const
  {
    std::stringstream ss;
    ss << std::hex << ((uint8_t) (this->red / 256) << 24 | (uint8_t) (this->green / 256) << 16 | (uint8_t) (this->blue / 256) << 8 | (uint8_t) (this->alpha / 256));
    std::string ssc = ss.str();
    return std::string{"#"} + ssc;
  }
}
