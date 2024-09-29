#pragma once

#include "export.hh"
#include "glrRenderable.hh"

#include <functional>

namespace glr
{
  //A sortable container for Renderables
  struct RenderList
  {
    RenderList() = default;
    
    using Comparator = std::function<bool(const Renderable& a, const Renderable& b)>;
    GLRENDER_API static bool renderableComparator(const Renderable& a, const Renderable& b);
    
    GLRENDER_API RenderList& operator +(RenderList& other);
    GLRENDER_API RenderList& operator +=(RenderList& other);
    
    GLRENDER_API Renderable& operator [](size_t index);
    GLRENDER_API std::vector<Renderable>::iterator begin();
    GLRENDER_API std::vector<Renderable>::iterator end();
    GLRENDER_API Renderable& front();
    GLRENDER_API Renderable& back();
    GLRENDER_API void add(const std::initializer_list<Renderable>& renderables);
    GLRENDER_API void clear();
    GLRENDER_API void sort(const Comparator& cmp = renderableComparator);
    [[nodiscard]] GLRENDER_API bool empty() const;
    [[nodiscard]] GLRENDER_API size_t size() const;
    
    std::vector<Renderable> list{};
  };
}
