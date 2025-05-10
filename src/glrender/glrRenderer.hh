#pragma once

#if !defined(GLR_NO_RENDERER)
#include "glrPostProcessing.hh"
#include "glrColor.hh"
#include "glrMesh.hh"
#include "glrShader.hh"
#include "glrTexture.hh"
#include "glrRenderable.hh"
#include "glrRenderList.hh"
#include "glrEnums.hh"

#include <commons/math/mat4.hh>

namespace glr
{
  typedef void (*GLapiproc)();
  typedef GLapiproc (*GLLoadFunc)(const char* name);
  //typedef void (*GLLoadFunc)(const char* name);

  struct Alternator
  {
    bool swap();
    bool get() const; //true: a false: b
      
    private:
    bool alt = true;
  };

  //TODO support pipelines
  /// OpenGL 4.5+ rendering engine
  struct Renderer
  {
    /// You're expected to be using a windowing library like SDL or Qt, it will provide you with loadFunc
    /// @param loadFunc The OpenGL loading function provided by the windowing library
    /// @param contextWidth The OpenGL context's width
    /// @param contextHeight The OpenGL context's height
    GLRENDER_API Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight);
    
    GLRENDER_API ~Renderer();

    /// Render a list of objects
    /// @param renderList A list of data that can be used to render something
    /// @param viewMat The view matrix
    /// @param projectionMat The projection matrix
    GLRENDER_API void render(RenderList renderList, const mat4x4<float>& viewMat, const mat4x4<float>& projectionMat);

    /// Call this when the OpenGL context has changed size
    /// @param width The new width
    /// @param height The new height
    GLRENDER_API void onContextResize(uint32_t width, uint32_t height);

    /// Set the postprocessing effects chain that will be applied to all rendered objects
    /// @param stack The ordered list of postprocessing effects to apply
    GLRENDER_API void setGlobalPostStack(std::shared_ptr<PostStack> stack);

    /// Set the postprocessing effects chain that will be applied to objects on one layer
    /// @param layer The layer to set
    /// @param stack The ordered list of postprocessing effects to apply
    GLRENDER_API void setLayerPostStack(uint64_t layer, std::shared_ptr<PostStack> stack);
    
    GLRENDER_API void useBackBuffer() const;
    GLRENDER_API void setClearColor(Color color) const;
    GLRENDER_API void clearCurrentFramebuffer() const;
    GLRENDER_API void setScissorTest(bool val) const;
    GLRENDER_API void setDepthTest(bool val) const;
    GLRENDER_API void setBlending(bool val) const;
    GLRENDER_API void setBlendMode(uint32_t src, uint32_t dst) const;
    GLRENDER_API void setCullFace(bool val) const;
    GLRENDER_API void setFilterMode(FilterMode min, FilterMode mag);
    GLRENDER_API void draw(GLDrawMode mode, size_t numElements) const;
    
    /// Bind an image for use in a compute shader
    GLRENDER_API void bindImage(uint32_t target, uint32_t handle, IOMode mode, GLColorFormat format) const;
    
    /// Run the currently bound compute shader
    GLRENDER_API void startComputeShader(const vec2<uint32_t>& contextSize, const vec2<uint32_t>& workSize = {WORK_SIZE_X, WORK_SIZE_Y}) const;
    
    GLRENDER_API static uint32_t WORK_SIZE_X;
    GLRENDER_API static uint32_t WORK_SIZE_Y;
    
    FramebufferPool fboPool{};
    
    private:
    void pingPong();
    void renderWithoutPost(const RenderList& renderList, std::shared_ptr<Texture>& curTexture);
    void renderWithPost(RenderList& renderList, std::shared_ptr<Texture>& curTexture);
    void postProcessGlobal();
    void postProcessLayer(uint64_t layer);
    void drawToScratch() const;
    void drawToBackBuffer() const;
    void scratchToPingPong();
    void drawRenderable(const Renderable& entry);

    vec2<uint32_t> contextSize{};
    
    FilterMode filterModeMin{};
    FilterMode filterModeMag{};
    
    std::shared_ptr<PostStack> globalPostStack = nullptr;
    std::unordered_map<uint64_t, std::shared_ptr<PostStack>> layerPostStack{};
    
    mat4x4<float> model{};
    mat4x4<float> view{};
    mat4x4<float> projection{};
    mat4x4<float> mvp{};

    Alternator curFBO{};
    Framebuffer fboA{};
    Framebuffer fboB{};
    Framebuffer scratch{};
    
    std::unique_ptr<Mesh> fullscreenQuad{};
    std::unique_ptr<Shader> shaderTransfer{};
  };
}
#endif
