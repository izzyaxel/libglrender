#pragma once

#include "glrPostProcessing.hh"
#include "glrColor.hh"
#include "glrMesh.hh"
#include "glrShader.hh"
#include "glrTexture.hh"
#include "glrAtlas.hh"
#include "glrRenderable.hh"
#include "glrRenderList.hh"

#include "commons/math/mat4.hh"

namespace glr
{
  typedef void (*GLapiproc)(void);
  typedef GLapiproc (*GLLoadFunc)(const char *name);
  //typedef void (*GLLoadFunc)(const char *name);
  
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
    GLRENDER_API static void startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize = {WORKSIZEX, WORKSIZEY});
    
    std::unique_ptr<FramebufferPool> fboPool = nullptr;
    
    GLRENDER_API static uint32_t WORKSIZEX;
    GLRENDER_API static uint32_t WORKSIZEY;
    
    private:
    struct Alternator
    {
      inline bool swap()
      {
        alt = !alt;
        return alt;
      }
      
      [[nodiscard]] inline bool get() const
      {
        return alt;
      } //true: a false: b
      
      private:
      bool alt = true;
    };
    
    void postProcessGlobal();
    void postProcessLayer(uint64_t layer);
    void drawToScratch();
    void drawToBackBuffer();
    void scratchToPingPong();
    void drawRenderable(Renderable &entry);
    
    std::shared_ptr<PostStack> globalPostStack;
    std::unordered_map<uint64_t, std::shared_ptr<PostStack>> layerPostStack;
    mat4x4<float> model = {};
    mat4x4<float> view = {};
    mat4x4<float> projection = {};
    mat4x4<float> mvp = {};
    std::shared_ptr<Framebuffer> fboA = nullptr;
    std::shared_ptr<Framebuffer> fboB = nullptr;
    std::shared_ptr<Framebuffer> scratch = nullptr;
    std::unique_ptr<Mesh> fullscreenQuad = nullptr;
    std::unique_ptr<Shader> shaderTransfer = nullptr;
    std::unique_ptr<Shader> shaderText = nullptr;
    Alternator curFBO = {};
  };
}
