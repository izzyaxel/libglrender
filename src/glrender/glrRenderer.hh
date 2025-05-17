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











namespace glr
{
  struct PipelineRenderer;

  using ID = uint64_t;
  inline constexpr static ID INVALID_ID = std::numeric_limits<uint64_t>::max();
  
  /// A list of instructions to execute in order to render things
  struct Pipeline
  {
    friend PipelineRenderer;
    
    GLRENDER_API Pipeline();
    GLRENDER_API void setClearColor(const vec4<float>& color);
    GLRENDER_API void setClearDepth(float value);
    GLRENDER_API void setClearStencil(int32_t value);
    GLRENDER_API void clearCurrentFramebuffer();
    
    GLRENDER_API void bindTexture(ID texture, uint32_t target);
    GLRENDER_API void bindImage(ID texture, uint32_t target, GLRIOMode a, GLRColorFormat b);
    GLRENDER_API void bindShader(ID shader);
    GLRENDER_API void bindMesh(ID mesh);
    GLRENDER_API void bindFramebuffer(ID framebuffer);
    GLRENDER_API void bindFramebufferAttachment(ID framebuffer, uint32_t target, GLRAttachment a, GLRAttachmentType b);
    GLRENDER_API void bindShaderPipeline(ID shaderPipeline);
    
    GLRENDER_API void setUniformFloat(ID shader, const std::string& name, float value);
    GLRENDER_API void setUniformU8(ID shader, const std::string& name, uint32_t value);
    GLRENDER_API void setUniformI8(ID shader, const std::string& name, uint32_t value);
    GLRENDER_API void setUniformU16(ID shader, const std::string& name, uint32_t value);
    GLRENDER_API void setUniformI16(ID shader, const std::string& name, uint32_t value);
    GLRENDER_API void setUniformU32(ID shader, const std::string& name, uint32_t value);
    GLRENDER_API void setUniformI32(ID shader, const std::string& name, int32_t value);
    GLRENDER_API void setUniformVec2u(ID shader, const std::string& name, const vec2<uint32_t>& value);
    GLRENDER_API void setUniformVec2i(ID shader, const std::string& name, const vec2<int32_t>& value);
    GLRENDER_API void setUniformVec2f(ID shader, const std::string& name, const vec2<float>& value);
    GLRENDER_API void setUniformVec3u(ID shader, const std::string& name, const vec3<uint32_t>& value);
    GLRENDER_API void setUniformVec3i(ID shader, const std::string& name, const vec3<int32_t>& value);
    GLRENDER_API void setUniformVec3f(ID shader, const std::string& name, const vec3<float>& value);
    GLRENDER_API void setUniformVec4u(ID shader, const std::string& name, const vec4<uint32_t>& value);
    GLRENDER_API void setUniformVec4i(ID shader, const std::string& name, const vec4<int32_t>& value);
    GLRENDER_API void setUniformVec4f(ID shader, const std::string& name, const vec4<float>& value);
    GLRENDER_API void setUniformMat3u(ID shader, const std::string& name, const mat3x3<uint32_t>& value);
    GLRENDER_API void setUniformMat3i(ID shader, const std::string& name, const mat3x3<int32_t>& value);
    GLRENDER_API void setUniformMat3f(ID shader, const std::string& name, const mat3x3<float>& value);
    GLRENDER_API void setUniformMat4u(ID shader, const std::string& name, const mat4x4<uint32_t>& value);
    GLRENDER_API void setUniformMat4i(ID shader, const std::string& name, const mat4x4<int32_t>& value);
    GLRENDER_API void setUniformMat4f(ID shader, const std::string& name, const mat4x4<float>& value);
    GLRENDER_API void sendUniforms(ID shader);
    
    GLRENDER_API void setFilterMode(GLRFilterMode min, GLRFilterMode mag);
    GLRENDER_API void setCullBackfaces(bool enabled);
    GLRENDER_API void setBlend(bool enabled);
    GLRENDER_API void setBlendMode(GLRBlendMode src, GLRBlendMode dst);
    GLRENDER_API void setDepthTest(bool enabled);
    GLRENDER_API void setScissorTest(bool enabled);
    
    GLRENDER_API void dispatchCompute();
    GLRENDER_API void draw(GLRDrawMode a, uint64_t numVertices);
    GLRENDER_API void drawIndexed(GLRDrawMode a, uint64_t numIndices, GLRIndexBufferType b);

    //Settings
    uint32_t workSizeX = 40;
    uint32_t workSizeY = 20;
    
    private:
    enum struct OpCode : uint16_t
    {
      INVALID = 0,
      SET_CLEAR_COLOR, SET_CLEAR_DEPTH, SET_CLEAR_STENCIL, CLEAR,
      USE_TEX, USE_IMG, USE_SHADER, USE_MESH, USE_FBO, USE_ATTACH, USE_PIPELINE,
      SET_UNI_F, SET_UNI_U8, SET_UNI_I8, SET_UNI_U16, SET_UNI_I16, SET_UNI_U32, SET_UNI_I32,
      SET_UNI_VEC2U, SET_UNI_VEC2I, SET_UNI_VEC2F,
      SET_UNI_VEC3U, SET_UNI_VEC3I, SET_UNI_VEC3F,
      SET_UNI_VEC4U, SET_UNI_VEC4I, SET_UNI_VEC4F,
      SET_UNI_MAT3U, SET_UNI_MAT3I, SET_UNI_MAT3F,
      SET_UNI_MAT4U, SET_UNI_MAT4I, SET_UNI_MAT4F,
      SEND_UNIFORMS,
      DRAW, DRAW_INDEXED, DISPATCH_COMPUTE,
      SET_FILTER_MODE, SET_CULL_BACKFACE, SET_BLEND, SET_BLEND_MODE, SET_DEPTH_TEST, SET_SCISSOR_TEST,
      
    };
    
    struct Instruction
    {
      OpCode op = OpCode::INVALID;
      ID id = INVALID_ID;
      uint32_t target = 0;
      uint32_t enumA = 0;
      uint32_t enumB = 0;
      uint32_t enumC = 0;
      std::string name;
      
      union Data
      {
        bool varBool;
        float varF;
        uint8_t varU8;
        int8_t varI8;
        uint16_t varU16;
        int16_t varI16;
        uint32_t varU32;
        int32_t varI32;
        uint64_t varU64;
        int64_t varI64;
        
        vec2<uint32_t> varVec2u;
        vec2<int32_t> varVec2i;
        vec2<float> varVec2f;
        
        vec3<uint32_t> varVec3u;
        vec3<int32_t> varVec3i;
        vec3<float> varVec3f;
        
        vec4<uint32_t> varVec4u;
        vec4<int32_t> varVec4i;
        vec4<float> varVec4f;

        mat3x3<uint32_t> varMat3u;
        mat3x3<int32_t> varMat3i;
        mat3x3<float> varMat3f;
        
        mat4x4<uint32_t> varMat4u;
        mat4x4<int32_t> varMat4i;
        mat4x4<float> varMat4f;
      } data;
    };

    std::vector<Instruction> instructions{};
    
    std::vector<ID> currentTexture{};
    ID currentShader = INVALID_ID;
    ID currentMesh = INVALID_ID;
    ID currentFramebuffer = INVALID_ID;
    ID currentAttachment = INVALID_ID;
    ID currentShaderPipeline = INVALID_ID;
  };

  struct PipelineRenderer
  {
    GLRENDER_API PipelineRenderer();
    GLRENDER_API ID addPipeline(const Pipeline& pipeline);
    GLRENDER_API void usePipeline(ID pipeline);
    GLRENDER_API void render();

    uint32_t contextSizeX = 800;
    uint32_t contextSizeY = 600;
    int32_t maxTextureUnitsPerStage = 0;
    int32_t maxTextureUnits = 0;
    
    private:
    ID currentPipeline = INVALID_ID;

    ID lastPipeline = 0;
    std::unordered_map<ID, Pipeline> pipelines{};
  };
}
