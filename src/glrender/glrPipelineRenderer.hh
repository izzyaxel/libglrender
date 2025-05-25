#pragma once

#include "export.hh"
#include "glrAssetID.hh"
#include "glrEnums.hh"
#include "glrLogging.hh"

#include <commons/math/vec4.hh>
#include <commons/math/mat3.hh>
#include <commons/math/mat4.hh>
#include <functional>

namespace glr
{
  typedef void (*GLapiproc)();
  typedef GLapiproc (*GLLoadFunc)(const char* name);
  //typedef void (*GLLoadFunc)(const char* name);
  
  struct PipelineRenderer;
  
  /// A list of instructions to execute in order to render things
  struct Pipeline
  {
    friend PipelineRenderer;

    using MatrixCallback = std::function<mat4x4<float>()>;
    
    GLRENDER_API Pipeline();

    //Instruction recording
    GLRENDER_API void setModelMatrix(const MatrixCallback& callback);
    GLRENDER_API void setViewMatrix(const MatrixCallback& callback);
    GLRENDER_API void setPerspectiveProjectionMatrix(const MatrixCallback& callback);
    GLRENDER_API void setOrthoProjectionMatrix(const MatrixCallback& callback);
    GLRENDER_API void calculateMVP();
    GLRENDER_API void setClearColor(const vec4<float>& color);
    GLRENDER_API void setClearDepth(float value);
    GLRENDER_API void setClearStencil(int32_t value);
    GLRENDER_API void clearCurrentFramebuffer(GLRClearType a, GLRClearType b = GLRClearType::NONE, GLRClearType c = GLRClearType::NONE);
    GLRENDER_API void bindTexture(ID texture, uint32_t target);
    GLRENDER_API void bindImage(ID texture, uint32_t target, GLRIOMode a, GLRColorFormat b);
    GLRENDER_API void bindShader(ID shader);
    GLRENDER_API void bindMesh(ID mesh);
    GLRENDER_API void bindBackbuffer();
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
    GLRENDER_API void setUniformMat3f(ID shader, const std::string& name, const mat3x3<float>& value);
    GLRENDER_API void setUniformMat4f(ID shader, const std::string& name, const mat4x4<float>& value);
    GLRENDER_API void setUniformMVP(ID shader);
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
      SET_MODEL_MATRIX, SET_VIEW_MATRIX, SET_ORTHO_PROJECTION_MATRIX, SET_PERSP_PROJECTION_MATRIX, CALCULATE_MVP,
      SET_CLEAR_COLOR, SET_CLEAR_DEPTH, SET_CLEAR_STENCIL, CLEAR,
      USE_TEX, USE_IMG, USE_SHADER, USE_MESH, USE_FBO, USE_ATTACH, USE_PIPELINE, USE_BACKBUFFER,
      SET_UNI_F, SET_UNI_U8, SET_UNI_I8, SET_UNI_U16, SET_UNI_I16, SET_UNI_U32, SET_UNI_I32,
      SET_UNI_VEC2U, SET_UNI_VEC2I, SET_UNI_VEC2F,
      SET_UNI_VEC3U, SET_UNI_VEC3I, SET_UNI_VEC3F,
      SET_UNI_VEC4U, SET_UNI_VEC4I, SET_UNI_VEC4F,
      SET_UNI_MAT3F, SET_UNI_MAT4F, SET_UNI_MVP, SEND_UNIFORMS,
      DRAW, DRAW_INDEXED, DISPATCH_COMPUTE,
      SET_FILTER_MODE, SET_CULL_BACKFACE, SET_BLEND, SET_BLEND_MODE, SET_DEPTH_TEST, SET_SCISSOR_TEST,
      
    };
    
    struct Instruction
    {
      OpCode op = OpCode::INVALID;
      ID id = INVALID_ID; //Asset ID
      uint32_t target = 0; //Binding target, mostly for textures
      uint32_t enumA = 0;
      uint32_t enumB = 0;
      uint32_t enumC = 0;
      std::string name; //Uniform name
      MatrixCallback matrixCallback = nullptr;
      
      union Data //Any other data the instruction will need
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
        
        mat3x3<float> varMat3f;
        mat4x4<float> varMat4f;
      } data;
    };

    std::vector<Instruction> instructions{};
    
    std::vector<ID> currentTexture{}; //Bound texture array, size is set to OpenGL's max textures per shader stage
    ID currentShader = INVALID_ID;
    ID currentMesh = INVALID_ID;
    ID currentFramebuffer = INVALID_ID;
    ID currentAttachment = INVALID_ID;
    ID currentShaderPipeline = INVALID_ID;
  };
  
  /// OpenGL 4.5+ modular forward rendering engine
  struct PipelineRenderer
  {
    GLRENDER_API PipelineRenderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight, const LoggingCallback& callback = nullptr);
    GLRENDER_API ID addPipeline(const Pipeline& pipeline);
    GLRENDER_API void usePipeline(ID pipeline);
    GLRENDER_API void render();

    uint32_t contextSizeX = 800;
    uint32_t contextSizeY = 600;
    int32_t maxTextureUnitsPerStage = 0;
    int32_t maxTextureUnits = 0;

    mat4x4<float> model{};
    mat4x4<float> view{};
    mat4x4<float> projection{};
    mat4x4<float> mvp{};

    vec2<uint32_t> viewport{};
    
    private:
    ID currentPipeline = INVALID_ID;

    ID lastPipeline = 0;
    std::unordered_map<ID, Pipeline> pipelines{};
  };
}
