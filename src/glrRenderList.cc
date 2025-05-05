#include "GLRender/glrRenderList.hh"

namespace glr
{
  bool RenderList::renderableComparator(const RenderableID& a, const RenderableID& b)
  {
    bool textures = false;
    bool layers = false;
    if(hasComp(a, TEXTURE) && hasComp(b, TEXTURE))
    {
      textures = getTextureComp(a)->texture->handle > getTextureComp(b)->texture->handle;
    }
    if(hasComp(a, LAYER) && hasComp(b, LAYER))
    {
      const auto& aLayerComp = getLayerComp(a);
      const auto& bLayerComp = getLayerComp(b);
      layers = aLayerComp->layer == bLayerComp->layer ? aLayerComp->sublayer > bLayerComp->sublayer : aLayerComp->layer > bLayerComp->layer;
    }
    return textures && layers;
  }
  
  RenderList::~RenderList()
  {
    if(this->shaderPipeline.has_value())
    {
      this->shaderPipeline = {};
    }
    for(const RenderableID& id : this->list)
    {
      removeRenderable(id);
    }
  }
  
  RenderList::RenderList(const RenderList& copyFrom)
  {
    this->list = copyFrom.list;
    this->shaderPipeline = copyFrom.shaderPipeline;
  }
  
  RenderList& RenderList::operator=(const RenderList& copyFrom)
  {
    if(this == &copyFrom)
    {
      return *this;
    }
    this->list = copyFrom.list;
    this->shaderPipeline = copyFrom.shaderPipeline;
    return *this;
  }
  
  RenderList::RenderList(RenderList&& moveFrom) noexcept
  {
    this->list = std::move(moveFrom.list);
    moveFrom.list = {};
    this->shaderPipeline = std::move(moveFrom.shaderPipeline);
    moveFrom.shaderPipeline = {};
  }
  
  RenderList& RenderList::operator=(RenderList&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->list = std::move(moveFrom.list);
    moveFrom.list = {};
    this->shaderPipeline = std::move(moveFrom.shaderPipeline);
    moveFrom.shaderPipeline = {};
    return *this;
  }
  
  RenderList& RenderList::operator +(const RenderList& other)
  {
    this->list.insert(this->list.end(), other.list.begin(), other.list.end());
    return *this;
  }
  
  RenderList& RenderList::operator +=(const RenderList& other)
  {
    this->list.insert(this->list.end(), other.list.begin(), other.list.end());
    return *this;
  }
  
  RenderableID &RenderList::operator [](const size_t index)
  {
    return this->list[index];
  }
  
  std::vector<RenderableID>::iterator RenderList::begin()
  {
    return this->list.begin();
  }
  
  std::vector<RenderableID>::iterator RenderList::end()
  {
    return this->list.end();
  }

  RenderableID& RenderList::front()
  {
    return this->list.front();
  }

  RenderableID& RenderList::back()
  {
    return this->list.back();
  }

  void RenderList::add(const RenderableID renderable)
  {
    this->list.emplace_back(renderable);
  }
  
  void RenderList::add(const std::initializer_list<RenderableID> renderables)
  {
    this->list.insert(this->list.end(), renderables.begin(), renderables.end());
  }
  
  void RenderList::clear()
  {
    this->list.clear();
  }
  
  bool RenderList::empty() const
  {
    return this->list.empty();
  }
  
  size_t RenderList::size() const
  {
    return this->list.size();
  }
  
  void RenderList::sort(const Comparator& cmp)
  {
    std::sort(this->list.begin(), this->list.end(), cmp);
  }
  
  void RenderList::reset()
  {
    this->clear();
    this->shaderPipeline.reset();
  }
}
