#pragma once

#include "export.hh"
#include "glrRenderable.hh"

#include <functional>

namespace glr
{
  /// A sortable list structure for Renderables
  struct RenderList
  {
    RenderList() = default;
    
    using Comparator = std::function<bool(Renderable const &a, Renderable const &b)>;
    GLRENDER_API static bool renderableComparator(Renderable const &a, Renderable const &b);
    
    GLRENDER_API RenderList& operator +(RenderList &other);
    GLRENDER_API RenderList& operator +=(RenderList &other);
    
    GLRENDER_API Renderable& operator [](size_t index);
    GLRENDER_API auto begin();
    GLRENDER_API auto end();
    GLRENDER_API void add(std::initializer_list<Renderable> const &renderables);
    GLRENDER_API void clear();
    [[nodiscard]] GLRENDER_API bool empty() const;
    [[nodiscard]] GLRENDER_API size_t size() const;
    GLRENDER_API void sort(Comparator const &cmp = renderableComparator);
    
    std::vector<Renderable> list{};
  };
}
