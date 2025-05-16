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
    GLRENDER_API void setFilterMode(GLRFilterMode min, GLRFilterMode mag);
    GLRENDER_API void draw(GLRDrawMode mode, size_t numVerticies) const;
    GLRENDER_API void drawIndexed(GLRDrawMode mode, size_t numIndices) const;
    
    /// Bind an image for use in a compute shader
    GLRENDER_API void bindImage(uint32_t target, uint32_t handle, GLRIOMode mode, GLRColorFormat format) const;
    
    /// Run the currently bound compute shader
    GLRENDER_API void startComputeShader(const vec2<uint32_t>& contextSize, const vec2<uint32_t>& workSize = {WORK_SIZE_X, WORK_SIZE_Y}) const;
    
    GLRENDER_API inline static uint32_t WORK_SIZE_X = 40;
    GLRENDER_API inline static uint32_t WORK_SIZE_Y = 20;
    
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








  
  using ID = uint64_t;
  inline constexpr static ID INVALID_ID = std::numeric_limits<uint64_t>::max();
  
  /// A list of instructions to execute to render things
  struct Pipeline
  {
    void clearCurrentFramebuffer();
    
    void bindTexture(ID texture, uint32_t target);
    void bindImage(ID texture, uint32_t target, GLRIOMode ioMode, GLRColorFormat format);
    void bindShader(ID shader);
    void bindMesh(ID mesh);
    void bindFramebuffer(ID framebuffer);
    void bindFramebufferAttachment(ID framebuffer, uint32_t target, GLRAttachment attachment, GLRAttachmentType type);
    void bindShaderPipeline(ID shaderPipeline);

    void setUniformFloat(ID shader, const std::string& name, float value);
    void setUniformU32(ID shader, const std::string& name, uint32_t value);
    void setUniformI32(ID shader, const std::string& name, int32_t value);
    template <typename T> void setUniformVec2(ID shader, const std::string& name, vec2<T> value)
    {
      
    }
    template <typename T> void setUniformVec3(ID shader, const std::string& name, vec3<T> value)
    {
      
    }
    template <typename T> void setUniformVec4(ID shader, const std::string& name, vec4<T> value)
    {
      
    }
    template <typename T> void setUniformMat3(ID shader, const std::string& name, mat3x3<T> value)
    {
      
    }
    template <typename T> void setUniformMat4(ID shader, const std::string& name, mat4x4<T> value)
    {
      
    }
    
    void sendUniforms(ID shader);

    private:
    typedef enum : uint32_t
    {
      INVALID = 0,
      CLEAR = 1,
      BIND_TEX = 2, BIND_IMG = 3, BIND_SHADER = 4, BIND_MESH = 5, BIND_FBO = 6, BIND_ATTACH = 7, BIND_PIPELINE = 8,
      SET_UNI_F = 10, SET_UNI_U32 = 11, SET_UNI_I32 = 12, SET_UNI_VEC2 = 13, SET_UNI_VEC3 = 14, SET_UNI_VEC4 = 15, SET_UNI_MAT2 = 16, SET_UNI_MAT3 = 17, SET_UNI_MAT4 = 18,
      SEND_UNIS = 20,
      
    } OpCode;

    //TODO mappings to tell the renderer what command to execute and where the data for that command is located in the buffers
    struct Instruction
    {
      OpCode op = INVALID;
      
    };

    std::vector<Instruction> instructions{};
    
    ID currentTexture = INVALID_ID;
    ID currentShader = INVALID_ID;
    ID currentMesh = INVALID_ID;
    ID currentFramebuffer = INVALID_ID;
    ID currentAttachment = INVALID_ID;
    ID currentShaderPipeline = INVALID_ID;

    //TODO data mappings
  };

  struct PipelineRenderer
  {
    ID addPipeline(const Pipeline& pipeline);
    void usePipeline(ID pipeline);
    void render();
    
    private:
    ID currentPipeline = INVALID_ID;

    ID lastPipeline = 0;
    std::unordered_map<ID, Pipeline> pipelines{};
  };
}
