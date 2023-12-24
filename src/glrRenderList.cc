#include "glrRenderList.hh"

namespace glr
{
  bool RenderList::renderableComparator(Renderable const &a, Renderable const &b)
  {
    return (a.texture.handle > b.texture.handle) && (a.layer == b.layer) ? a.sublayer > b.sublayer : a.layer > b.layer;
  }
  
  RenderList& RenderList::operator +(RenderList &other)
  {
    this->list.insert(this->list.end(), other.list.begin(), other.list.end());
    return *this;
  }
  
  RenderList& RenderList::operator +=(RenderList &other)
  {
    this->list.insert(this->list.end(), other.list.begin(), other.list.end());
    return *this;
  }
  
  Renderable &RenderList::operator [](size_t index)
  {
    return this->list[index];
  }
  
  auto RenderList::begin()
  {
    return this->list.begin();
  }
  
  auto RenderList::end()
  {
    return this->list.end();
  }
  
  void RenderList::add(std::initializer_list<Renderable> const &renderables)
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
  
  void RenderList::sort(Comparator const &cmp)
  {
    std::sort(this->list.begin(), this->list.end(), cmp);
  }
}
