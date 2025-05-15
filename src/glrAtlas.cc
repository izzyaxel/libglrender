#include "glrender/glrAtlas.hh"

#include <glad/gl.hh>
#include <algorithm>

namespace glr
{
  struct BSPLayout
  {
    BSPLayout(const uint32_t initWidth, const uint32_t initHeight)
    {
      this->root = new BSPNode(false, initWidth, initHeight);
    }
    
    BSPLayout() : BSPLayout(0, 0)
    {}
    
    ~BSPLayout()
    {
      delete this->root;
    }
    
    vec2<uint32_t> pack(const uint32_t width, const uint32_t height)
    {
      if(width == 0 || height == 0)
      {
        printf("BSPLayout error: Attempted to pack a tile with 0 width or height\n");
        return {};
      }
      vec2<uint32_t> out = {};
      bool ok;
      this->root->packIter(ok, width, height, out);
      if(!ok)
      {
        if(this->root->width + width > this->root->height + height)
        {
          BSPNode* newRoot = new BSPNode(false, std::max(this->root->width, width), this->root->height + height);
          newRoot->childA = this->root;
          newRoot->childB = new BSPNode(false, std::max(this->root->width, width), height, 0, this->root->height);
          newRoot->packIter(ok, width, height, out);
          this->root = newRoot;
        }
        else
        {
          BSPNode* newRoot = new BSPNode(false, this->root->width + width, std::max(this->root->height, height));
          newRoot->childA = this->root;
          newRoot->childB = new BSPNode(false, width, std::max(this->root->height, height), this->root->width, 0);
          newRoot->packIter(ok, width, height, out);
          this->root = newRoot;
        }
      }
      return out;
    }
    
    [[nodiscard]] uint32_t width() const
    {
      return this->root->width;
    }
    
    [[nodiscard]] uint32_t height() const
    {
      return this->root->height;
    }
    
    private:
    struct BSPNode
    {
      BSPNode(const bool isEndpoint, const uint32_t width, const uint32_t height, const uint32_t x, const uint32_t y) :
        width(width), height(height), coords(x, y), isEndpoint(isEndpoint)
      {}
      
      BSPNode(const bool isEndpoint, const uint32_t width, const uint32_t height) :
        BSPNode(isEndpoint, width, height, 0, 0)
      {}
      
      ~BSPNode()
      {
        if(this->childA && this->childB)
        {
          delete this->childA;
          delete this->childB;
        }
      }
      
      const uint32_t width = 0;
      const uint32_t height = 0;
      vec2<uint32_t> coords = {};
      bool isEndpoint = false;
      
      BSPNode* childA = nullptr;
      BSPNode* childB = nullptr;
      
      GLRENDER_API void packIter(bool& ok, const uint32_t width, const uint32_t height, vec2<uint32_t>& pos)
      {
        if(this->isEndpoint || (width > this->width) || (height > this->height))
        {
          ok = false;
          return;
        }
        
        if(this->childA && this->childB)
        {
          this->childA->packIter(ok, width, height, pos);
          if(ok) return;
          this->childB->packIter(ok, width, height, pos);
          return;
        }
        
        if(width == this->width && height == this->height)
        {
          this->isEndpoint = true;
          ok = true;
          pos = this->coords;
          return;
        }
        
        if(width != this->width && height != this->height)
        {
          this->childA = new BSPNode(false, width, this->height, this->coords.x(), this->coords.y());
          this->childB = new BSPNode(false, this->width - width, this->height, this->coords.x() + width, this->coords.y());
          this->childA->packIter(ok, width, height, pos);
          return;
        }
        
        if(width == this->width)
        {
          this->childA = new BSPNode(true, width, height, this->coords.x(), this->coords.y());
          this->childB = new BSPNode(false, width, this->height - height, this->coords.x(), this->coords.y() + height);
          ok = true;
          pos = this->childA->coords;
          return;
        }
        
        if(height == this->height)
        {
          this->childA = new BSPNode(true, width, height, this->coords.x(), this->coords.y());
          this->childB = new BSPNode(false, this->width - width, this->height, this->coords.x() + width, this->coords.y());
          ok = true;
          pos = this->childA->coords;
        }
      }
    };
    
    BSPNode* root = nullptr;
  };
  
  Atlas::Atlas(Atlas&& moveFrom) noexcept
  {
    this->atlas = moveFrom.atlas;
    moveFrom.atlas = {};
    
    this->atlasDims = moveFrom.atlasDims;
    moveFrom.atlasDims = {};
    
    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Atlas &Atlas::operator =(Atlas&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->atlas = moveFrom.atlas;
    moveFrom.atlas = {};
    
    this->atlasDims = moveFrom.atlasDims;
    moveFrom.atlasDims = {};
    
    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  void Atlas::addTile(const std::string& name, const std::vector<uint8_t>& tileData, const uint8_t channels, const uint32_t width, const uint32_t height)
  {
    if(this->contains(name))
    {
      printf("Atlas error: Atlas already contains a file with the name %s\n", name.c_str());
      return;
    }
    if(this->finalized)
    {
      printf("Atlas error: Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize\n");
      return;
    }
    if(tileData.empty())
    {
      printf("Atlas error: Tile data is empty\n");
      return;
    }
    this->atlas.emplace_back(name, tileData, channels, vec2<uint32_t>{0, 0}, width, height);
    this->init = true;
  }
  
  void Atlas::addTile(const std::string& name, const uint8_t channels, std::vector<uint8_t>&& tileData, const uint32_t width, const uint32_t height)
  {
    if(this->contains(name))
    {
      printf("Atlas error: Atlas already contains a tile with the name %s\n", name.c_str());
      return;
    }
    if(this->finalized)
    {
      printf("Atlas error: Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize\n");
      return;
    }
    if(tileData.empty())
    {
      printf("Atlas error: Tile data is empty\n");
      return;
    }
    this->atlas.emplace_back(name, std::move(tileData), channels, vec2<uint32_t>{0, 0}, width, height);
    this->init = true;
  }
  
  QuadUVs Atlas::getUVsForTile(const std::string& name)
  {
    if(!this->finalized || !this->contains(name))
    {
      return QuadUVs{};
    }
    vec2<uint32_t> location{};
    uint32_t width = 0, height = 0;
    for(auto& tile: this->atlas)
    {
      if(tile.name == name)
      {
        location = tile.location;
        width = tile.width;
        height = tile.height;
        break;
      }
    }
    vec2<float> ll = vec2{(float) location.x(), (float) location.y()};
    vec2<float> ul = vec2{(float) location.x(), (float) (location.y() + height)};
    vec2<float> lr = vec2{(float) (location.x() + width), (float) location.y()};
    vec2<float> ur = vec2{(float) (location.x() + width), (float) (location.y() + height)};
    ll = ll / this->atlasDims;
    ul = ul / this->atlasDims;
    lr = lr / this->atlasDims;
    ur = ur / this->atlasDims;
    return QuadUVs{ul, ll, ur, lr};
  }
  
  vec2<float> Atlas::getTileDimensions(const std::string& name)
  {
    if(!this->contains(name)) return {0.0f, 0.0f};
    for(const auto& tile: this->atlas)
    {
      if(tile.name == name)
      {
        return vec2{(float)tile.width, (float)tile.height};
      }
    }
    return vec2{0.0f, 0.0f};
  }
  
  void Atlas::use(const Texture& atlasTexture) const
  {
    if(!this->finalized)
    {
      printf("Atlas error: Trying to bind atlas before it's been finalized\n");
    }
    else
    {
      atlasTexture.use();
    }
  }
  
  bool Atlas::contains(const std::string& tileName)
  {
    for(const auto& img: this->atlas)
    {
      if(img.name == tileName)
      {
        return true;
      }
    }
    return false;
  }
  
  void Atlas::finalize(const std::string& name, Texture& atlasTexture, const uint8_t channels)
  {
    if(this->finalized)
    {
      printf("Atlas error: Atlas has already been uploaded to the GPU, finalization failed\n");
      return;
    }
    if(this->atlas.empty())
    {
      printf("Atlas error: Atlas doesn't contain anything, finalization failed\n");
      return;
    }
    BSPLayout layout;
    std::sort(this->atlas.begin(), this->atlas.end(), AtlasImg::comparator);
    for(auto& tile: this->atlas)
    {
      if(tile.width == 0 || tile.height == 0)
      {
        printf("Atlas error: Atlas encountered a tile with 0 width or height: %s finalization failed\n", tile.name.c_str());
        continue;
      }
      tile.location = layout.pack(tile.width, tile.height);
    }
    if(layout.height() == 0 || layout.width() == 0)
    {
      printf("Atlas error: After layout, this atlas would have 0 width or height, finalization failed\n");
      return;
    }
    
    atlasTexture.name = name;
    atlasTexture.height = layout.height();
    atlasTexture.width = layout.width();
    atlasTexture.channels = channels;
    atlasTexture.setFilterMode(GLRFilterMode::NEAREST, GLRFilterMode::NEAREST);
    
    atlasTexture = Texture(name, layout.width(), layout.height(), channels, GLRFilterMode::NEAREST);
    for(auto& tile: this->atlas)
    {
      atlasTexture.subImage(tile.data.data(), tile.width, tile.height, tile.location.x(), tile.location.y(), tile.channels);
    }
    this->atlasDims = {(float)layout.width(), (float)layout.height()};
    this->finalized = true;
    this->init = true;
  }
  
  bool Atlas::exists() const
  {
    return this->init;
  }
  
  void Atlas::reset()
  {
    this->atlas.clear();
    this->finalized = false;
    this->atlasDims = {};
    this->init = false;
  }
}
