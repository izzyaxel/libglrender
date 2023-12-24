#include "glrAtlas.hh"

#include <glad/gl.hh>
#include <algorithm>

namespace glr
{
  struct BSPLayout
  {
    BSPLayout(uint32_t initWidth, uint32_t initHeight)
    {
      this->p_root = new BSPNode(false, initWidth, initHeight);
    }
    
    BSPLayout() : BSPLayout(0, 0)
    {}
    
    ~BSPLayout()
    {
      delete this->p_root;
    }
    
    vec2<uint32_t> pack(uint32_t width, uint32_t height)
    {
      if(width == 0 || height == 0)
      {
        printf("BSPLayout error: Attempted to pack a tile with 0 width or height\n");
        return {};
      }
      vec2<uint32_t> out = {};
      bool ok;
      this->p_root->packIter(ok, width, height, out);
      if(!ok)
      {
        if(this->p_root->p_width + width > this->p_root->p_height + height)
        {
          BSPNode *newRoot = new BSPNode(false, std::max(this->p_root->p_width, width), this->p_root->p_height + height);
          newRoot->p_childA = this->p_root;
          newRoot->p_childB = new BSPNode(false, std::max(this->p_root->p_width, width), height, 0, this->p_root->p_height);
          newRoot->packIter(ok, width, height, out);
          this->p_root = newRoot;
        }
        else
        {
          BSPNode *newRoot = new BSPNode(false, this->p_root->p_width + width, std::max(this->p_root->p_height, height));
          newRoot->p_childA = this->p_root;
          newRoot->p_childB = new BSPNode(false, width, std::max(this->p_root->p_height, height), this->p_root->p_width, 0);
          newRoot->packIter(ok, width, height, out);
          this->p_root = newRoot;
        }
      }
      return out;
    }
    
    uint32_t width() const
    {
      return this->p_root->p_width;
    }
    
    uint32_t height() const
    {
      return this->p_root->p_height;
    }
    
    private:
    struct BSPNode
    {
      BSPNode(bool isEndpoint, uint32_t width, uint32_t height, uint32_t x, uint32_t y) :
        p_width(width), p_height(height), p_coords(x, y), p_isEndpoint(isEndpoint)
      {}
      
      BSPNode(bool isEndpoint, uint32_t width, uint32_t height) :
        BSPNode(isEndpoint, width, height, 0, 0)
      {}
      
      ~BSPNode()
      {
        if(this->p_childA && this->p_childB)
        {
          delete this->p_childA;
          delete this->p_childB;
        }
      }
      
      uint32_t const p_width = 0;
      uint32_t const p_height = 0;
      vec2<uint32_t> p_coords = {};
      bool p_isEndpoint = false;
      
      BSPNode *p_childA = nullptr;
      BSPNode *p_childB = nullptr;
      
      GLRENDER_API inline void packIter(bool &ok, uint32_t const &width, uint32_t const &height, vec2<uint32_t> &pos)
      {
        if(this->p_isEndpoint || (width > this->p_width) || (height > this->p_height))
        {
          ok = false;
          return;
        }
        
        if(this->p_childA && this->p_childB)
        {
          this->p_childA->packIter(ok, width, height, pos);
          if(ok) return;
          this->p_childB->packIter(ok, width, height, pos);
          return;
        }
        
        if(width == this->p_width && height == this->p_height)
        {
          this->p_isEndpoint = true;
          ok = true;
          pos = this->p_coords;
          return;
        }
        
        if(width != this->p_width && height != this->p_height)
        {
          this->p_childA = new BSPNode(false, width, this->p_height, this->p_coords.x(), this->p_coords.y());
          this->p_childB = new BSPNode(false, this->p_width - width, this->p_height, this->p_coords.x() + width, this->p_coords.y());
          this->p_childA->packIter(ok, width, height, pos);
          return;
        }
        
        if(width == this->p_width)
        {
          this->p_childA = new BSPNode(true, width, height, this->p_coords.x(), this->p_coords.y());
          this->p_childB = new BSPNode(false, width, this->p_height - height, this->p_coords.x(), this->p_coords.y() + height);
          ok = true;
          pos = this->p_childA->p_coords;
          return;
        }
        
        if(height == this->p_height)
        {
          this->p_childA = new BSPNode(true, width, height, this->p_coords.x(), this->p_coords.y());
          this->p_childB = new BSPNode(false, this->p_width - width, this->p_height, this->p_coords.x() + width, this->p_coords.y());
          ok = true;
          pos = this->p_childA->p_coords;
          return;
        }
      }
    };
    
    BSPNode *p_root = nullptr;
  };
  
  Atlas::~Atlas()
  {
    this->atlasTexture.reset();
  }
  
  Atlas::Atlas(Atlas &&moveFrom) noexcept
  {
    this->atlas = moveFrom.atlas;
    moveFrom.atlas = {};
    
    this->atlasTexture = std::move(moveFrom.atlasTexture);
    moveFrom.atlasTexture = {};
    
    this->atlasDims = moveFrom.atlasDims;
    moveFrom.atlasDims = {};
    
    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Atlas &Atlas::operator =(glr::Atlas &&moveFrom) noexcept
  {
    this->atlas = moveFrom.atlas;
    moveFrom.atlas = {};
    
    this->atlasTexture = std::move(moveFrom.atlasTexture);
    moveFrom.atlasTexture = {};
    
    this->atlasDims = moveFrom.atlasDims;
    moveFrom.atlasDims = {};
    
    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  void Atlas::addTile(std::string const &name, std::vector<uint8_t> const &tileData, TexColorFormat format, uint32_t width, uint32_t height)
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
    this->atlas.emplace_back(name, tileData, format, vec2<uint32_t>{0, 0}, width, height);
    this->init = true;
  }
  
  void Atlas::addTile(std::string const &name, TexColorFormat fmt, std::vector<uint8_t> &&tileData, uint32_t width, uint32_t height)
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
    this->atlas.emplace_back(name, std::move(tileData), fmt, vec2<uint32_t>{0, 0}, width, height);
    this->init = true;
  }
  
  QuadUVs Atlas::getUVsForTile(std::string const &name)
  {
    if(!this->finalized || !this->contains(name))
    {
      return QuadUVs{};
    }
    vec2<uint32_t> location{};
    uint32_t width = 0, height = 0;
    for(auto &tile: this->atlas)
    {
      if(tile.name == name)
      {
        location = tile.location;
        width = tile.width;
        height = tile.height;
        break;
      }
    }
    vec2<float> ll = vec2<float>{(float) location.x(), (float) location.y()};
    vec2<float> ul = vec2<float>{(float) location.x(), (float) (location.y() + height)};
    vec2<float> lr = vec2<float>{(float) (location.x() + width), (float) location.y()};
    vec2<float> ur = vec2<float>{(float) (location.x() + width), (float) (location.y() + height)};
    ll = ll / this->atlasDims;
    ul = ul / this->atlasDims;
    lr = lr / this->atlasDims;
    ur = ur / this->atlasDims;
    return QuadUVs{ul, ll, ur, lr};
  }
  
  vec2<float> Atlas::getTileDimensions(std::string const &name)
  {
    if(!this->contains(name)) return {0.0f, 0.0f};
    for(auto const &tile: this->atlas)
    {
      if(tile.name == name)
      {
        return vec2<float>{(float)tile.width, (float)tile.height};
      }
    }
    return vec2<float>{0.0f, 0.0f};
  }
  
  void Atlas::use(uint32_t target) const
  {
    if(!this->finalized)
    {
      printf("Atlas error: Trying to bind atlas before it's been finalized\n");
    }
    else
    {
      this->atlasTexture.use(target);
    }
  }
  
  bool Atlas::contains(std::string const &tileName)
  {
    for(auto const &img: this->atlas)
    {
      if(img.name == tileName)
      {
        return true;
      }
    }
    return false;
  }
  
  void Atlas::finalize(std::string const &name, TexColorFormat fmt)
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
    for(auto &tile: this->atlas)
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
    this->atlasTexture = Texture(Texture(name, layout.width(), layout.height(), fmt, FilterMode::NEAREST));
    this->atlasTexture.clear();
    for(auto &tile: this->atlas)
    {
      this->atlasTexture.subImage(tile.data.data(), tile.width, tile.height, tile.location.x(), tile.location.y(), tile.fmt);
    }
    this->atlasDims = {(float) layout.width(), (float) layout.height()};
    this->finalized = true;
    this->init = true;
  }
  
  bool Atlas::exists() const
  {
    return this->init;
  }
  
  void Atlas::reset()
  {
    this->atlasTexture.reset();
    this->atlas.clear();
    this->finalized = false;
    this->atlasDims = {};
    this->init = false;
  }
}
