#include "glrender/glrRenderList.hh"

#include <algorithm>

namespace glr
{
  bool RenderList::renderableComparator(const Renderable& a, const Renderable& b)
  {
    bool textures = false;
    bool layers = false;
    if(a.textureComp && b.textureComp)
    {
      textures = a.textureComp->texture->handle > b.textureComp->texture->handle;
    }
    if(a.layerComp && b.layerComp)
    {
      layers = a.layerComp->layer == b.layerComp->layer ? a.layerComp->sublayer > b.layerComp->sublayer : a.layerComp->layer > b.layerComp->layer;
    }
    return textures && layers;
  }
  
  RenderList::~RenderList()
  {
    if(this->shaderPipeline.has_value())
    {
      this->shaderPipeline = {};
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
  
  Renderable &RenderList::operator [](const size_t index)
  {
    return this->list[index];
  }
  
  std::vector<Renderable>::iterator RenderList::begin()
  {
    return this->list.begin();
  }
  
  std::vector<Renderable>::iterator RenderList::end()
  {
    return this->list.end();
  }

  Renderable& RenderList::front()
  {
    return this->list.front();
  }

  Renderable& RenderList::back()
  {
    return this->list.back();
  }

  void RenderList::add(const Renderable renderable)
  {
    this->list.emplace_back(renderable);
  }
  
  void RenderList::add(const std::initializer_list<Renderable> renderables)
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
