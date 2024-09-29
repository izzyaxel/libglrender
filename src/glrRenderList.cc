#include "glrRenderList.hh"

namespace glr
{
  bool RenderList::renderableComparator(const Renderable& a, const Renderable& b)
  {
    return (a.texture->handle > b.texture->handle) && (a.layer == b.layer) ? a.sublayer > b.sublayer : a.layer > b.layer;
  }
  
  RenderList& RenderList::operator +(RenderList& other)
  {
    this->list.insert(this->list.end(), other.list.begin(), other.list.end());
    return *this;
  }
  
  RenderList& RenderList::operator +=(RenderList& other)
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
  
  void RenderList::add(const std::initializer_list<Renderable>& renderables)
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
}
