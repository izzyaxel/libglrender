#pragma once

#include "export.hh"
#include "glrRenderable.hh"
#include "glrShaderPipeline.hh"

#include <functional>
#include <optional>

namespace glr
{
  /// A sortable container for Renderables
  struct RenderList
  {
    GLRENDER_API RenderList() = default;
    GLRENDER_API ~RenderList();
    
    GLRENDER_API RenderList(const RenderList& copyFrom);
    GLRENDER_API RenderList& operator=(const RenderList& copyFrom);
    GLRENDER_API RenderList(RenderList&& moveFrom) noexcept;
    GLRENDER_API RenderList& operator=(RenderList&& moveFrom) noexcept;
    
    using Comparator = std::function<bool(const Renderable& a, const Renderable& b)>;
    GLRENDER_API static bool renderableComparator(const Renderable& a, const Renderable& b);
    
    GLRENDER_API RenderList& operator +(const RenderList& other);
    GLRENDER_API RenderList& operator +=(const RenderList& other);
    GLRENDER_API Renderable& operator [](size_t index);
    
    GLRENDER_API std::vector<Renderable>::iterator begin();
    GLRENDER_API std::vector<Renderable>::iterator end();
    GLRENDER_API Renderable& front();
    GLRENDER_API Renderable& back();
    GLRENDER_API void add(Renderable renderable);
    GLRENDER_API void add(std::initializer_list<Renderable> renderables);
    
    /// Clear the render list
    GLRENDER_API void clear();
    
    /// Clear the render list and delete the pipeline
    GLRENDER_API void reset();
    
    GLRENDER_API void sort(const Comparator& cmp = renderableComparator);
    [[nodiscard]] GLRENDER_API bool empty() const;
    [[nodiscard]] GLRENDER_API size_t size() const;
    
    /// Optional.  If used, then all Renderables in this list will use this pipeline instead of their individual shaders
    bool useShaderPipeline = false;
    std::optional<ShaderPipeline> shaderPipeline{};
    
    std::vector<Renderable> list{};
  };
}
