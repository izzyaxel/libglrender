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
    /// You're expected to be using a windowing library like SDL or Qt, it will provide you with @param loadFunc
    GLRENDER_API Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight);
    GLRENDER_API ~Renderer();
    
    GLRENDER_API void onContextResize(uint32_t width, uint32_t height);
    GLRENDER_API void setGlobalPostStack(std::shared_ptr<PostStack> stack);
    GLRENDER_API void setLayerPostStack(uint64_t layer, std::shared_ptr<PostStack> stack);
    GLRENDER_API void useBackBuffer() const;
    GLRENDER_API void render(RenderList renderList, const mat4x4<float>& viewMat, const mat4x4<float>& projectionMat);
    GLRENDER_API void setClearColor(Color color) const;
    GLRENDER_API void clearCurrentFramebuffer() const;
    GLRENDER_API void setScissorTest(bool val) const;
    GLRENDER_API void setDepthTest(bool val) const;
    GLRENDER_API void setBlending(bool val) const;
    GLRENDER_API void setBlendMode(uint32_t src, uint32_t dst) const;
    GLRENDER_API void setCullFace(bool val) const;
    GLRENDER_API void setFilterMode(FilterMode mode) const;
    GLRENDER_API void draw(DrawMode mode, size_t numElements) const;
    GLRENDER_API void pingPong();
    
    GLRENDER_API void bindImage(uint32_t target, uint32_t handle, IOMode mode, GLColorFormat format) const;
    GLRENDER_API void startComputeShader(const vec2<uint32_t>& contextSize, const vec2<uint32_t>& workSize = {WORKSIZEX, WORKSIZEY}) const;
    
    FramebufferPool fboPool{};
    
    GLRENDER_API static uint32_t WORKSIZEX;
    GLRENDER_API static uint32_t WORKSIZEY;
    
    private:
    struct Alternator
    {
      bool swap()
      {
        alt = !alt;
        return alt;
      }
      
      [[nodiscard]] bool get() const
      {
        return alt;
      } //true: a false: b
      
      private:
      bool alt = true;
    };
    
    void renderWithoutPost(RenderList& renderList, const Texture* curTexture);
    void renderWithPost(RenderList& renderList, const Texture* curTexture);
    
    void postProcessGlobal();
    void postProcessLayer(uint64_t layer);
    void drawToScratch() const;
    void drawToBackBuffer() const;
    void scratchToPingPong();
    void drawRenderable(Renderable& entry);
    
    std::shared_ptr<PostStack> globalPostStack;
    std::unordered_map<uint64_t, std::shared_ptr<PostStack>> layerPostStack;
    mat4x4<float> model = {};
    mat4x4<float> view = {};
    mat4x4<float> projection = {};
    mat4x4<float> mvp = {};
    Framebuffer fboA{};
    Framebuffer fboB{};
    Framebuffer scratch{};
    Mesh fullscreenQuad{};
    Shader shaderTransfer{};
    Shader shaderText{};
    Alternator curFBO = {};
  };
}
