#pragma once

#include "export.hh"
#include "glrEnums.hh"
#include "glrUtil.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace glr
{
  struct DownloadedImageData
  {
    std::vector<uint8_t> imageData{};
    int32_t width = 0;
    int32_t height = 0;
    std::string textureName;
  };
  
  /// An on-VRAM OpenGL texture
  struct Texture
  {
    Texture() = default;
    
    /// Allocate VRAM for a texture without assigning data to it
    GLRENDER_API Texture(const std::string& name, uint32_t width, uint32_t height, TexColorFormat colorFormat, FilterMode mode = FilterMode::NEAREST, bool sRGB = false);
    
    /// Create a texture from a flat array
    GLRENDER_API Texture(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height, TexColorFormat colorFormat, FilterMode mode = FilterMode::NEAREST, bool sRGB = false);
    
    /// Generates a single color 1x1 texture
    GLRENDER_API explicit Texture(const std::string& name, uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255, bool sRGB = false);
    
    GLRENDER_API ~Texture();
    
    Texture(const Texture& copyFrom) = delete;
    Texture& operator=(const Texture& copyFrom) = delete;
    GLRENDER_API Texture(Texture&& moveFrom) noexcept;
    GLRENDER_API Texture& operator=(Texture&& moveFrom) noexcept;
    
    [[nodiscard]] GLRENDER_API bool isValid() const;
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use(uint32_t target) const;
    GLRENDER_API void setFilterMode(FilterMode min, FilterMode mag) const;
    GLRENDER_API void setAnisotropyLevel(uint32_t level) const;
    GLRENDER_API void subImage(const uint8_t* data, uint32_t w, uint32_t h, uint32_t xPos, uint32_t yPos, TexColorFormat format) const;
    GLRENDER_API void clear() const;
    
    [[nodiscard]] GLRENDER_API DownloadedImageData downloadTexture(TexColorFormat colorFormat) const;
    
    uint32_t handle = INVALID_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
    TexColorFormat fmt = {};
    std::string name;
    std::string path;
    
    private:
    bool init = false;
  };
}
