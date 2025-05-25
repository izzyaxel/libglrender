#include "glrender/glrPipelineRenderer.hh"
#include "glrender/glrAssetRepository.hh"

#include <glad/gl.hh>

namespace glr
{
  LoggingCallback cbPipeline = nullptr;
  void glPipelineDebug(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei messageLength, const GLchar* message, const void* userData)
  {
    if(!cbPipeline)
    {
      return;
    }
    
    std::string severityStr;
    std::string typeStr;
    std::string sourceStr;
    
    switch(source)
    {
      case GL_DEBUG_SOURCE_API:
      {
        sourceStr = "Source: OpenGL API";
        break;
      }
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      {
        sourceStr = "Source: Window-system API";
        break;
      }
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
      {
        sourceStr = "Source: Shader Compiler";
        break;
      }
      case GL_DEBUG_SOURCE_THIRD_PARTY:
      {
        sourceStr = "Source: Third-party Application";
        break;
      }
      case GL_DEBUG_SOURCE_APPLICATION:
      {
        sourceStr = "Source: User's Application";
        break;
      }
      case GL_DEBUG_SOURCE_OTHER:
      {
        sourceStr = "Source: Other";
        break;
      }
      default: break;
    }
    
    switch(severity)
    {
      case GL_DEBUG_SEVERITY_NOTIFICATION:
      {
        severityStr = "Severity: NOTIFICATION";
        break;
      }
      case GL_DEBUG_SEVERITY_LOW:
      {
        severityStr = "Severity: LOW";
        break;
      }
      case GL_DEBUG_SEVERITY_MEDIUM:
      {
        severityStr = "Severity: MEDIUM";
        break;
      }
      case GL_DEBUG_SEVERITY_HIGH:
      {
        severityStr = "Severity: HIGH";
        break;
      }
      default: break;
    }
    
    switch(type)
    {
      case GL_DEBUG_TYPE_ERROR:
      {
        typeStr = "Type: Error";
        break;
      }
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      {
        typeStr = "Type: Deprecated Behavior";
        break;
      }
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      {
        typeStr = "Type: Undefined Behavior";
        break;
      }
      case GL_DEBUG_TYPE_PORTABILITY:
      {
        typeStr = "Type: Portability";
        break;
      }
      case GL_DEBUG_TYPE_PERFORMANCE:
      {
        typeStr = "Type: Performance";
        break;
      }
      case GL_DEBUG_TYPE_MARKER:
      {
        typeStr = "Type: Command Stream Annotation";
        break;
      }
      case GL_DEBUG_TYPE_PUSH_GROUP:
      {
        typeStr = "Type: Group Pushing";
        break;
      }
      case GL_DEBUG_TYPE_POP_GROUP:
      {
        typeStr = "Type: Group Popping";
        break;
      }
      case GL_DEBUG_TYPE_OTHER:
      {
        typeStr = "Type: Other";
        break;
      }
      default: break;
    }
    cbPipeline(GLRLogType::ERROR, "An OpenGL error occured: [" + sourceStr + "] " + severityStr + ", ID: " + std::to_string(id) + ", " + typeStr + ", Message: " + message + "\n");
  }
  
  template <typename... Args> void lg(const std::string& msg, Args... args)
  {
    #if 0
    printf(msg.c_str(), args...);
    #endif
  }
  
  Pipeline::Pipeline()
  {
    int32_t maxTextures;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
    this->currentTexture.resize(maxTextures, INVALID_ID);
  }
  
  void Pipeline::setModelMatrix(const MatrixCallback& callback)
  {
    this->instructions.emplace_back(OpCode::SET_MODEL_MATRIX);
    this->instructions.back().matrixCallback = callback;
  }
  
  void Pipeline::setViewMatrix(const MatrixCallback& callback)
  {
    this->instructions.emplace_back(OpCode::SET_VIEW_MATRIX);
    this->instructions.back().matrixCallback = callback;
  }
  
  void Pipeline::setPerspectiveProjectionMatrix(const MatrixCallback& callback)
  {
    this->instructions.emplace_back(OpCode::SET_PERSP_PROJECTION_MATRIX);
    this->instructions.back().matrixCallback = callback;
  }
  
  void Pipeline::setOrthoProjectionMatrix(const MatrixCallback& callback)
  {
    this->instructions.emplace_back(OpCode::SET_ORTHO_PROJECTION_MATRIX);
    this->instructions.back().matrixCallback = callback;
  }

  void Pipeline::calculateMVP()
  {
    this->instructions.emplace_back(OpCode::CALCULATE_MVP);
  }
  
  void Pipeline::setClearColor(const vec4<float>& color)
  {
    this->instructions.emplace_back(OpCode::SET_CLEAR_COLOR);
    this->instructions.back().data.varVec4f = color;
  }

  void Pipeline::setClearDepth(const float value)
  {
    this->instructions.emplace_back(OpCode::SET_CLEAR_DEPTH);
    this->instructions.back().data.varF = value;
  }

  void Pipeline::setClearStencil(const int32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_CLEAR_STENCIL);
    this->instructions.back().data.varI32 = value;
  }
  
  void Pipeline::clearCurrentFramebuffer(const GLRClearType a, const GLRClearType b, const GLRClearType c)
  {
    this->instructions.emplace_back(OpCode::CLEAR);
    this->instructions.back().enumA = (uint32_t)a;
    this->instructions.back().enumB = (uint32_t)b;
    this->instructions.back().enumC = (uint32_t)c;
  }

  void Pipeline::bindTexture(const ID texture, const uint32_t target)
  {
    this->instructions.emplace_back(OpCode::USE_TEX, texture, target);
  }

  void Pipeline::bindImage(const ID texture, const uint32_t target, const GLRIOMode a, const GLRColorFormat b)
  {
    this->instructions.emplace_back(OpCode::USE_IMG, texture, target, (uint16_t)a, (uint16_t)b);
  }
  
  void Pipeline::bindMesh(const ID mesh)
  {
    this->instructions.emplace_back(OpCode::USE_MESH, mesh);
  }

  void Pipeline::bindShader(const ID shader)
  {
    this->instructions.emplace_back(OpCode::USE_SHADER, shader);
  }

  void Pipeline::bindBackbuffer()
  {
    this->instructions.emplace_back(OpCode::USE_BACKBUFFER);
  }

  void Pipeline::bindFramebuffer(const ID framebuffer)
  {
    this->instructions.emplace_back(OpCode::USE_FBO, framebuffer);
  }

  void Pipeline::bindFramebufferAttachment(const ID framebuffer, const uint32_t target, const GLRAttachment a, const GLRAttachmentType b)
  {
    this->instructions.emplace_back(OpCode::USE_ATTACH, framebuffer, target, (uint16_t)a, (uint16_t)b);
  }

  void Pipeline::bindShaderPipeline(const ID shaderPipeline)
  {
    this->instructions.emplace_back(OpCode::USE_PIPELINE, shaderPipeline);
  }

  void Pipeline::setUniformFloat(const ID shader, const std::string& name, const float value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varF = value;
  }

  void Pipeline::setUniformU8(const ID shader, const std::string& name, const uint32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_U8, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varU8 = value;
  }
  
  void Pipeline::setUniformI8(const ID shader, const std::string& name, const uint32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_I8, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varI8 = value;
  }
  
  void Pipeline::setUniformU16(const ID shader, const std::string& name, const uint32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_U16, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varU16 = value;
  }
  
  void Pipeline::setUniformI16(const ID shader, const std::string& name, const uint32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_I16, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varI16 = value;
  }
  
  void Pipeline::setUniformU32(const ID shader, const std::string& name, const uint32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_U32, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varU32 = value;
  }

  void Pipeline::setUniformI32(const ID shader, const std::string& name, const int32_t value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_I32, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varI32 = value;
  }

  
  void Pipeline::setUniformVec2u(ID shader, const std::string& name, const vec2<uint32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC2U, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec2u = value;
  }
  
  void Pipeline::setUniformVec2i(ID shader, const std::string& name, const vec2<int32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC2I, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec2i = value;
  }
  
  void Pipeline::setUniformVec2f(ID shader, const std::string& name, const vec2<float>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC2F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec2f = value;
  }


  void Pipeline::setUniformVec3u(ID shader, const std::string& name, const vec3<uint32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC3U, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec3u = value;
  }
  
  void Pipeline::setUniformVec3i(ID shader, const std::string& name, const vec3<int32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC3I, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec3i = value;
  }
  
  void Pipeline::setUniformVec3f(ID shader, const std::string& name, const vec3<float>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC3F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec3f = value;
  }


  void Pipeline::setUniformVec4u(ID shader, const std::string& name, const vec4<uint32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC4U, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec4u = value;
  }
  
  void Pipeline::setUniformVec4i(ID shader, const std::string& name, const vec4<int32_t>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC4I, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec4i = value;
  }
  
  void Pipeline::setUniformVec4f(ID shader, const std::string& name, const vec4<float>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_VEC4F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varVec4f = value;
  }
  
  void Pipeline::setUniformMat3f(ID shader, const std::string& name, const mat3x3<float>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_MAT3F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varMat3f = value;
  }
  
  void Pipeline::setUniformMat4f(ID shader, const std::string& name, const mat4x4<float>& value)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_MAT4F, shader, 0, 0, 0, 0, name);
    this->instructions.back().data.varMat4f = value;
  }

  void Pipeline::setUniformMVP(ID shader)
  {
    this->instructions.emplace_back(OpCode::SET_UNI_MVP, shader);
    this->instructions.back().name = "mvp";
  }

  
  void Pipeline::sendUniforms(const ID shader)
  {
    this->instructions.emplace_back(OpCode::SEND_UNIFORMS, shader);
  }

  void Pipeline::dispatchCompute()
  {
    this->instructions.emplace_back(OpCode::DISPATCH_COMPUTE);
  }

  void Pipeline::draw(const GLRDrawMode a, const uint64_t numVertices)
  {
    this->instructions.emplace_back(OpCode::DRAW, INVALID_ID, 0, (uint16_t)a);
    this->instructions.back().data.varU64 = numVertices;
  }

  void Pipeline::drawIndexed(const GLRDrawMode a, const uint64_t numIndices, const GLRIndexBufferType b)
  {
    this->instructions.emplace_back(OpCode::DRAW_INDEXED, INVALID_ID, 0, (uint16_t)a, (uint16_t)b);
    this->instructions.back().data.varU64 = numIndices;
  }

  void Pipeline::setFilterMode(const GLRFilterMode min, const GLRFilterMode mag)
  {
    this->instructions.emplace_back(OpCode::SET_FILTER_MODE, INVALID_ID, 0, (uint16_t)min, (uint16_t)mag);
  }

  void Pipeline::setBlend(const bool enabled)
  {
    this->instructions.emplace_back(OpCode::SET_BLEND);
    this->instructions.back().data.varBool = enabled;
  }

  void Pipeline::setBlendMode(const GLRBlendMode src, const GLRBlendMode dst)
  {
    this->instructions.emplace_back(OpCode::SET_BLEND_MODE, INVALID_ID, 0, (uint16_t)src, (uint16_t)dst);
  }

  void Pipeline::setDepthTest(const bool enabled)
  {
    this->instructions.emplace_back(OpCode::SET_DEPTH_TEST);
    this->instructions.back().data.varBool = enabled;
  }
  
  void Pipeline::setScissorTest(const bool enabled)
  {
    this->instructions.emplace_back(OpCode::SET_SCISSOR_TEST);
    this->instructions.back().data.varBool = enabled;
  }

  void Pipeline::setCullBackfaces(const bool enabled)
  {
    this->instructions.emplace_back(OpCode::SET_CULL_BACKFACE);
    this->instructions.back().data.varBool = enabled;
  }
  

  PipelineRenderer::PipelineRenderer(const GLLoadFunc loadFunc, const uint32_t contextWidth, const uint32_t contextHeight, const LoggingCallback& callback)
  {
    gladLoadGL(loadFunc);
    this->viewport = {contextWidth, contextHeight};
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glPipelineDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    glViewport(0, 0, (GLsizei)contextWidth, (GLsizei)contextHeight);
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->maxTextureUnitsPerStage);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &this->maxTextureUnits);

    cbPipeline = callback;
  }
  
  ID PipelineRenderer::addPipeline(const Pipeline& pipeline)
  {
    const ID out = this->lastPipeline;
    this->lastPipeline++;
    this->pipelines[out] = pipeline;
    return out;
  }

  void PipelineRenderer::usePipeline(const ID pipeline)
  {
    if(!this->pipelines.contains(pipeline))
    {
      return;
    }
    this->currentPipeline = pipeline;
  }

  void PipelineRenderer::render()
  {
    auto& curPipeline = this->pipelines.at(this->currentPipeline);
    for(const auto& [op, id, target, enumA, enumB, enumC, name, matrixCallback, data] : curPipeline.instructions)
    {
      switch(op)
      {
        case Pipeline::OpCode::SET_MODEL_MATRIX:
        {
          lg("Set model");
          this->model = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_ORTHO_PROJECTION_MATRIX:
        {
          lg("Set ortho projection");
          this->projection = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_PERSP_PROJECTION_MATRIX:
        {
          lg("Set perspective projection");
          this->projection = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_VIEW_MATRIX:
        {
          lg("Set view");
          this->view = matrixCallback();
          break;
        }
        case Pipeline::OpCode::CALCULATE_MVP:
        {
          lg("Calculate MVP");
          this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_COLOR:
        {
          lg("Set color clear value\n");
          const auto& c = data.varVec4f;
          glClearColor(c.r(), c.g(), c.b(), c.a());
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_DEPTH:
        {
          lg("Set depth clear value\n");
          glClearDepth(data.varF);
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_STENCIL:
        {
          lg("Set stencil clear value\n");
          glClearStencil(data.varI32);
          break;
        }
        case Pipeline::OpCode::CLEAR:
        {
          lg("Clear\n");
          glClear(enumA | enumB | enumC);
          break;
        }

        case Pipeline::OpCode::USE_ATTACH:
        {
          lg("Use FBO attachment %zu, 0x%04x, 0x%04x, %u\n", id, enumA, enumB, target);
          asset_repo::fboBindAttachment(id, (GLRAttachment)enumA, (GLRAttachmentType)enumB, target);
          break;
        }
        case Pipeline::OpCode::USE_TEX:
        {
          auto& tex = curPipeline.currentTexture;
          if(target >= tex.size() || tex.at(target) == id)
          {
            lg("Use texture: already bound\n");
            continue;
          }
          lg("Use texture\n");
          tex.at(target) = id;
          asset_repo::textureSetBindingTarget(id, target);
          asset_repo::textureUse(id);
          break;
        }
        case Pipeline::OpCode::USE_IMG: //TODO support layered images/levels
        {
          if(target >= curPipeline.currentTexture.size() || curPipeline.currentTexture.at(target) == id)
          {
            lg("Use image: already bound\n");
            continue;
          }
          lg("Use image\n");
          curPipeline.currentTexture.at(target) = id;
          asset_repo::textureUseAsImage(id, target, (GLRIOMode)enumA, (GLRColorFormat)enumB);
          break;
        }
        case Pipeline::OpCode::USE_SHADER:
        {
          if(curPipeline.currentShader == id)
          {
            lg("Use shader: already bound\n");
            continue;
          }
          lg("Use shader\n");
          curPipeline.currentShader = id;
          asset_repo::shaderUse(id);
          break;
        }
        case Pipeline::OpCode::USE_MESH:
        {
          if(curPipeline.currentMesh == id)
          {
            lg("Use mesh: already bound\n");
            continue;
          }
          lg("Use mesh\n");
          curPipeline.currentMesh = id;
          asset_repo::meshUse(id);
          break;
        }
        case Pipeline::OpCode::USE_BACKBUFFER:
        {
          lg("Use backbuffer\n");
          asset_repo::fboUse(0);
          break;
        }
        case Pipeline::OpCode::USE_FBO:
        {
          if(curPipeline.currentFramebuffer == id)
          {
            lg("Use framebuffer: already bound\n");
            continue;
          }
          lg("Use framebuffer\n");
          curPipeline.currentFramebuffer = id;
          asset_repo::fboUse(id);
          break;
        }
        case Pipeline::OpCode::USE_PIPELINE:
        {
          if(curPipeline.currentShaderPipeline == id)
          {
            lg("Use shader pipeline: already bound\n");
            continue;
          }
          lg("Use shader pipeline\n");
          curPipeline.currentShaderPipeline = id;
          asset_repo::shaderPipelineUse(id);
          break;
        }

        case Pipeline::OpCode::SET_UNI_F:
        {
          lg("Set float uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varF);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U8:
        {
          lg("Set u8 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU8);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I8:
        {
          lg("Set i8 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI8);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U16:
        {
          lg("Set u16 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU16);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I16:
        {
          lg("Set i16 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI16);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U32:
        {
          lg("Set u32 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU32);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I32:
        {
          lg("Set i32 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI32);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC2U:
        {
          lg("Set vec2u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC2I:
        {
          lg("Set vec2i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC2F:
        {
          lg("Set vec2f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC3U:
        {
          lg("Set vec3u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC3I:
        {
          lg("Set vec3i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC3F:
        {
          lg("Set vec3f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC4U:
        {
          lg("Set vec4u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC4I:
        {
          lg("Set vec4i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC4F:
        {
          lg("Set vec4f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_MAT3F:
        {
          lg("Set mat3x3f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varMat3f);
          break;
        }
        case Pipeline::OpCode::SET_UNI_MAT4F:
        {
          lg("Set mat4x4f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varMat4f);
          break;
        }
        case Pipeline::OpCode::SET_UNI_MVP:
        {
          lg("Set MVP uniform\n");
          asset_repo::shaderSetUniform(id, name, this->mvp);
          break;
        }

        case Pipeline::OpCode::SEND_UNIFORMS:
        {
          lg("Send uniforms\n");
          asset_repo::shaderSendUniforms(id);
          break;
        }

        case Pipeline::OpCode::DRAW:
        {
          lg("Draw %zu vertices with mode 0x%04x\n", data.varU64, enumA);
          glDrawArrays((GLenum)enumA, 0, data.varU64);
          break;
        }
        case Pipeline::OpCode::DRAW_INDEXED:
        {
          lg("Draw %zu indices with draw mode 0x%04x, index buffer format 0x%04x\n", data.varU64, enumA, enumB);
          glDrawElements((GLenum)enumA, data.varU64, (int32_t)enumB, nullptr);
          break;
        }
        case Pipeline::OpCode::DISPATCH_COMPUTE:
        {
          lg("Dispatch compute shader\n");
          glDispatchCompute((uint32_t)(std::ceil((float)(this->contextSizeX) / (float)curPipeline.workSizeX)), (uint32_t)(std::ceil((float)(this->contextSizeY) / (float)curPipeline.workSizeY)), 1);
          break;
        }

        case Pipeline::OpCode::SET_FILTER_MODE:
        {
          lg("Set filter modes\n");
          switch((GLRFilterMode)enumA)
          {
            case GLRFilterMode::NEAREST:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
              break;
            }
              
            case GLRFilterMode::BILINEAR:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              break;
            }
              
            case GLRFilterMode::TRILINEAR:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
              break;
            }
          }
          
          switch((GLRFilterMode)enumB)
          {
            case GLRFilterMode::NEAREST:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
              break;
            }
              
            case GLRFilterMode::BILINEAR:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
            }
              
            case GLRFilterMode::TRILINEAR:
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
              break;
            }
          }
          break;
        }
          
        case Pipeline::OpCode::SET_BLEND:
        {
          lg("Set blend\n");
          data.varBool ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
          break;
        }
        case Pipeline::OpCode::SET_BLEND_MODE:
        {
          lg("Set blend modes\n");
          glBlendFunc((int32_t)enumA, (int32_t)enumB);
          break;
        }
        case Pipeline::OpCode::SET_DEPTH_TEST:
        {
          lg("Set depth testing\n");
          data.varBool ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
          break;
        }
        case Pipeline::OpCode::SET_CULL_BACKFACE:
        {
          lg("Set backface culling\n");
          data.varBool ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
          break;
        }
        case Pipeline::OpCode::SET_SCISSOR_TEST:
        {
          lg("Set scissor testing\n");
          data.varBool ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
          break;
        }
          
        case Pipeline::OpCode::INVALID:
        default: lg("Invalid opcode\n"); break;
      }
    }
    lg("\n");
  }
}
