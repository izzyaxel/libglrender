#pragma once

#include "glrPostProcessing.hh"
#include "glrColor.hh"
#include "glrMesh.hh"
#include "glrShader.hh"
#include "glrTexture.hh"
#include "glrAtlas.hh"
#include "glrRenderable.hh"

#include "commons/math/mat4.hh"

#include <cstdint>
#include <functional>

namespace glr
{
  typedef void (*GLapiproc)(void);
  typedef GLapiproc (*GLLoadFunc)(const char *name);
  //typedef void (*GLLoadFunc)(const char *name);
  
  /// A sortable list structure for Renderables
  struct RenderList
  {
    using Comparator = std::function<bool(Renderable const &a, Renderable const &b)>;
    GLRENDER_API static bool renderableComparator(Renderable const &a, Renderable const &b);
    
    GLRENDER_API RenderList operator +(RenderList const &other);
    GLRENDER_API RenderList &operator +=(RenderList const &other);
    
    GLRENDER_API Renderable &operator [](size_t index);
    GLRENDER_API auto begin();
    GLRENDER_API auto end();
    GLRENDER_API void add(std::initializer_list<Renderable> const &renderables);
    GLRENDER_API void clear();
    [[nodiscard]] GLRENDER_API bool empty() const;
    [[nodiscard]] GLRENDER_API size_t size() const;
    GLRENDER_API void sort(Comparator const &cmp = renderableComparator);
    
    std::vector<Renderable> m_list;
  };
  
  /// The OpenGL 4.5+ rendering engine
  struct Renderer
  {
    /// You're expected to be using a windowing library like SDL2, it will provide you
    /// with @param loadFunc
    GLRENDER_API Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight);
    GLRENDER_API ~Renderer();
    
    GLRENDER_API void onContextResize(uint32_t width, uint32_t height);
    GLRENDER_API void setGlobalPostStack(std::shared_ptr<PostStack> stack);
    GLRENDER_API void setLayerPostStack(uint64_t layer, std::shared_ptr<PostStack> stack);
    GLRENDER_API void useBackBuffer();
    GLRENDER_API void render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection);
    GLRENDER_API void setClearColor(Color color);
    GLRENDER_API void clearCurrentFramebuffer();
    GLRENDER_API void setScissorTest(bool val);
    GLRENDER_API void setDepthTest(bool val);
    GLRENDER_API void setBlending(bool val);
    GLRENDER_API void setBlendMode(uint32_t src, uint32_t dst);
    GLRENDER_API void setCullFace(bool val);
    GLRENDER_API void setFilterMode(FilterMode mode);
    GLRENDER_API void draw(DrawMode mode, size_t numElements);
    GLRENDER_API void pingPong();
    
    GLRENDER_API static void bindImage(uint32_t target, uint32_t const &handle, IOMode mode, GLColorFormat format);
    GLRENDER_API static void startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize = {s_workSizeX, s_workSizeY});
    
    std::unique_ptr<FramebufferPool> m_fboPool = nullptr;
    
    GLRENDER_API static uint32_t s_workSizeX;
    GLRENDER_API static uint32_t s_workSizeY;
    
    private:
    struct Alternator
    {
      inline bool swap()
      {
        p_alt = !p_alt;
        return p_alt;
      }
      
      [[nodiscard]] inline bool get() const
      {
        return p_alt;
      } //true: a false: b
      
      private:
      bool p_alt = true;
    };
    
    void postProcessGlobal();
    void postProcessLayer(uint64_t layer);
    void drawToScratch();
    void drawToBackBuffer();
    void scratchToPingPong();
    void drawRenderable(Renderable const &entry);
    
    std::shared_ptr<PostStack> p_globalPostStack;
    std::unordered_map<uint64_t, std::shared_ptr<PostStack>> p_layerPostStack;
    mat4x4<float> p_model = {};
    mat4x4<float> p_view = {};
    mat4x4<float> p_projection = {};
    mat4x4<float> p_mvp = {};
    std::shared_ptr<Framebuffer> p_fboA = nullptr;
    std::shared_ptr<Framebuffer> p_fboB = nullptr;
    std::shared_ptr<Framebuffer> p_scratch = nullptr;
    std::unique_ptr<Mesh> p_fullscreenQuad = nullptr;
    std::unique_ptr<Shader> p_shaderTransfer = nullptr;
    std::unique_ptr<Shader> p_shaderText = nullptr;
    Alternator p_curFBO = {};
  };
}
