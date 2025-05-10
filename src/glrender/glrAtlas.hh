#pragma once

#include "export.hh"
#include "glrTexture.hh"
#include "glrMesh.hh"

#include <commons/math/vec2.hh>
#include <vector>
#include <string>
#include <memory>

namespace glr
{
  /// An on-VRAM atlas of stitched together images as one OpenGL texture
  struct Atlas
  {
    GLRENDER_API Atlas() = default;
    
    Atlas(Atlas const &copyFrom) = delete;
    Atlas& operator=(Atlas const &copyFrom) = delete;
    GLRENDER_API Atlas(Atlas &&moveFrom) noexcept;
    GLRENDER_API Atlas& operator=(Atlas &&moveFrom) noexcept;
    
    /// Add a new tile into this atlas
    /// \param name The name of the tile
    /// \param tileData Flat array of pixel data
    /// \param width The width of the new tile
    /// \param height The height of the new tile
    GLRENDER_API void addTile(const std::string& name, const std::vector<uint8_t>& tileData, ColorFormat format, uint32_t width, uint32_t height);
    
    /// Add a new tile into this atlas from raw pixel data
    GLRENDER_API void addTile(const std::string& name, ColorFormat fmt, std::vector<uint8_t>&& tileData, uint32_t width, uint32_t height);
    
    /// Get the UV coordinates in the atlas for the given tile
    /// \return UV coordinates
    [[nodiscard]] GLRENDER_API QuadUVs getUVsForTile(const std::string& name);
    
    [[nodiscard]] GLRENDER_API vec2<float> getTileDimensions(std::string const &name);
    
    /// Bind this atlas for rendering use
    GLRENDER_API void use(const Texture& atlasTexture, uint32_t target = 0) const;
    
    /// Check if this atlas contains a tile of the given name
    [[nodiscard]] GLRENDER_API bool contains(const std::string& tileName);
    
    /// Create the atlas and send it to the GPU
    GLRENDER_API void finalize(const std::string& name, Texture& atlasTexture, ColorFormat fmt);
    
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    
    private:
    struct AtlasImg
    {
      GLRENDER_API AtlasImg() = default;
      GLRENDER_API AtlasImg(std::string name, std::vector<uint8_t> data, const ColorFormat fmt, const vec2<uint32_t> location, const uint32_t width, const uint32_t height) :
        name(std::move(name)), data(std::move(data)), fmt(fmt), location(location), width(width), height(height)
      {}
      
      [[nodiscard]] GLRENDER_API static bool comparator(const AtlasImg& a, const AtlasImg& b)
      {
        return a.height * a.width > b.height * b.width;
      }
      
      std::string name;
      std::vector<uint8_t> data = {};
      ColorFormat fmt = RGBA;
      vec2<uint32_t> location = {};
      uint32_t width = 0;
      uint32_t height = 0;
    };
    
    vec2<float> atlasDims = {};
    std::vector<AtlasImg> atlas = {};
    bool finalized = false;
    bool init = false;
  };
}
