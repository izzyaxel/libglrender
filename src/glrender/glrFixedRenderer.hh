#pragma once

#include "glrPostProcessing.hh"
#include "glrColor.hh"
#include "glrMesh.hh"
#include "glrShader.hh"
#include "glrTexture.hh"
#include "glrRenderable.hh"
#include "glrRenderList.hh"
#include "glrEnums.hh"

#include <commons/math/mat4.hh>
#include <numeric>

//A fixed function pipeline rendering engine
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

  //TODO support shader pipelines
  /// OpenGL 4.5+ fixed function forward rendering engine
  struct Renderer
  {
    /// You're expected to be using a windowing library like SDL or Qt, it will provide you with loadFunc
    /// @param loadFunc The OpenGL loading function provided by the windowing library
    /// @param contextWidth The OpenGL context's width
    /// @param contextHeight The OpenGL context's height
    /// @param callback Error callback to redirect OpenGL errors into your project through
    GLRENDER_API Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight, const LoggingCallback& callback = nullptr);
    
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
    GLRENDER_API void setFilterMode(GLRFilterMode min, GLRFilterMode mag);
    /// Bind an image for use in a compute shader
    GLRENDER_API void bindImage(uint32_t target, uint32_t handle, GLRIOMode mode, GLRColorFormat format) const;

    /// Render the currently bound OpenGL objects, called by Renderer::render()
    /// @param mode The format the geometry is in
    /// @param numVerticies The number of vertices to render
    GLRENDER_API void draw(GLRDrawMode mode, size_t numVerticies) const;

    /// Render the currently bound OpenGL objects, called by Renderer::render()
    /// @param mode The format the geometry is in
    /// @param numIndices The number of indices to render
    GLRENDER_API void drawIndexed(GLRDrawMode mode, size_t numIndices) const;
    
    /// Run the currently bound compute shader
    GLRENDER_API void startComputeShader(const vec2<uint32_t>& contextSize) const;
    
    uint32_t workSizeX = 40;
    uint32_t workSizeY = 20;
    
    private:
    void pingPong();
    void renderWithoutLayerPost(const RenderList& rl, std::shared_ptr<Texture>& currentTexture);
    void renderWithLayerPost(RenderList& rl, std::shared_ptr<Texture>& currentTexture);
    void postProcessGlobal();
    void postProcessLayer(uint64_t layer);
    void drawToScratch() const;
    void drawToBackBuffer() const;
    void scratchToPingPong();
    void drawRenderable(const Renderable& entry);

    vec2<uint32_t> contextSize{};
    
    GLRFilterMode filterModeMin{};
    GLRFilterMode filterModeMag{};
    
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
