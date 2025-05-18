#include "glrender/glrRenderer.hh"
#include "glrender/glrAssetRepository.hh"

#include <glad/gl.hh>
#include <commons/math/mat4.hh>

namespace glr
{
//===Data===========================================================================
  std::string transferFrag =
R"(#version 460 core

in vec2 uv;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
  fragColor = texture(tex, uv);
})";
  
  std::string transferVert =
R"(#version 460 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;
out vec2 uv;

void main()
{
  uv = uv_in;
  gl_Position = vec4(pos_in, 1.0);
})";
  
  std::array fullscreenQuadVerts{1.0f, -1.0f,  1.0f, 1.0f,  -1.0f, -1.0f,  -1.0f, 1.0f};
  std::array fullscreenQuadUVs{1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 1.0f};

  //===Debug===========================================================================
  //TODO find a way to pass this to the user through a LoggingCallback
  void glDebug(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei messageLength, const GLchar* message, const void* userData)
  {
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
    printf("An OpenGL error occured: [%s] %s, ID: %u, %s, Message: %s\n", sourceStr.c_str(), severityStr.c_str(), id, typeStr.c_str(), message);
  }

  //===Renderer===========================================================================
  bool Alternator::swap()
  {
    alt = !alt;
    return alt;
  }
  
  bool Alternator::get() const
  {
    return alt;
  } //true: a false: b
  
  Renderer::Renderer(const GLLoadFunc loadFunc, const uint32_t contextWidth, const uint32_t contextHeight)
  {
    gladLoadGL(loadFunc);
    this->contextSize = {contextWidth, contextHeight};
    
    this->fboA.setDimensions(contextWidth, contextHeight)->addColorAttachment(GLRAttachmentType::TEXTURE, 4)->finalize();
    this->fboB.setDimensions(contextWidth, contextHeight)->addColorAttachment(GLRAttachmentType::TEXTURE, 4)->finalize();
    this->scratch.setDimensions(contextWidth, contextHeight)->addColorAttachment(GLRAttachmentType::TEXTURE, 3)->finalize();
    
    this->fullscreenQuad = std::make_unique<Mesh>();
    this->fullscreenQuad->setPositionDimensions(GLRDimensions::TWO_DIMENSIONAL);
    this->fullscreenQuad->addPositions(fullscreenQuadVerts.data(), fullscreenQuadVerts.size())->addUVs(fullscreenQuadUVs.data(), fullscreenQuadUVs.size())->finalize();
    this->shaderTransfer = std::make_unique<Shader>("Transfer Shader", transferVert, transferFrag);
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, (GLsizei)contextWidth, (GLsizei)contextHeight);
  }
  
  Renderer::~Renderer()
  {
    this->fboA.clear();
    this->fboB.clear();
    this->scratch.clear();
    this->shaderTransfer.reset();
    this->globalPostStack.reset();
    this->layerPostStack.clear();
  }

  //===Renderer Configuration===========================================================================
  void Renderer::onContextResize(const uint32_t width, const uint32_t height)
  {
    this->contextSize = {width, height};
    this->useBackBuffer();
    glViewport(0, 0, (int32_t)width, (int32_t)height);
  }
  
  void Renderer::setGlobalPostStack(std::shared_ptr<PostStack> stack)
  {
    this->globalPostStack = std::move(stack);
  }
  
  void Renderer::setLayerPostStack(const uint64_t layer, std::shared_ptr<PostStack> stack)
  {
    this->layerPostStack[layer] = std::move(stack);
  }

  //===OpenGL Wrappers===========================================================================
  void Renderer::useBackBuffer() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void Renderer::setClearColor(const Color color) const
  {
    vec4<float> colorF = color.asRGBAf();
    glClearColor(colorF.r(), colorF.g(), colorF.b(), colorF.a());
  }
  
  void Renderer::clearCurrentFramebuffer() const
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  
  void Renderer::setScissorTest(const bool val) const
  {
    val ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
  }
  
  void Renderer::setDepthTest(const bool val) const
  {
    val ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  }
  
  void Renderer::setBlending(const bool val) const
  {
    val ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  }
  
  void Renderer::setBlendMode(const uint32_t src, const uint32_t dst) const
  {
    glBlendFunc(src, dst);
  }
  
  void Renderer::setCullFace(const bool val) const
  {
    val ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  }
  
  void Renderer::setFilterMode(const GLRFilterMode min, const GLRFilterMode mag)
  {
    this->filterModeMin = min;
    this->filterModeMag = mag;
    
    switch(min)
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
    
    switch(mag)
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
  }
  
  void Renderer::draw(const GLRDrawMode mode, const size_t numVerticies) const
  {
    glDrawArrays((GLenum)mode, 0, (GLsizei)numVerticies);
  }

  void Renderer::drawIndexed(const GLRDrawMode mode, const size_t numIndices) const
  {
    glDrawElements((GLenum)mode, (GLsizei)numIndices, GL_UNSIGNED_INT, nullptr);
  }
  
  void Renderer::bindImage(const uint32_t target, const uint32_t handle, const GLRIOMode mode, const GLRColorFormat format) const
  {
    glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
  }
  
  void Renderer::startComputeShader(const vec2<uint32_t>& contextSize) const
  {
    glDispatchCompute((uint32_t)(std::ceil((float)(contextSize.x()) / (float)this->workSizeX)), (uint32_t)(std::ceil((float)(contextSize.y()) / (float)this->workSizeY)), 1);
  }
  
  //===Rendering===========================================================================
  void Renderer::drawToBackBuffer() const
  {
    this->fullscreenQuad->use();
    this->useBackBuffer();
    this->clearCurrentFramebuffer();
    this->shaderTransfer->use();
    if(this->curFBO.get())
    {
      this->fboA.bindAttachment(GLRAttachment::COLOR, GLRAttachmentType::TEXTURE, 0);
    }
    else
    {
      this->fboB.bindAttachment(GLRAttachment::COLOR, GLRAttachmentType::TEXTURE, 0);
    }
    this->draw(GLRDrawMode::TRI_STRIPS, this->fullscreenQuad->numVerts);
  }

  void Renderer::render(RenderList renderList, const mat4x4<float>& viewMat, const mat4x4<float>& projectionMat)
  {
    RenderList rl = std::move(renderList);
    if(rl.empty())
    {
      return;
    }
    
    std::shared_ptr<Texture> currentTexture = nullptr;
    this->view = viewMat;
    this->projection = projectionMat;
    
    if(rl.front().textureComp && rl.front().textureComp->texture)
    {
      currentTexture = rl.front().textureComp->texture;
      currentTexture->use();
    }

    const bool doPostprocessing = !this->layerPostStack.empty() || (this->globalPostStack && !this->globalPostStack->isEmpty());

    if(!doPostprocessing) //TODO FIXME can't render directly to the back buffer, why? get "no defined base level" warnings on textures, most likely the FBO attachments?
    {
      //TODO ideally, draw directly to the backbuffer
      this->pingPong();
      this->renderWithoutLayerPost(rl, currentTexture);
      this->drawToBackBuffer();
    }
    else
    {
      if(!this->layerPostStack.empty())
      {
        this->renderWithLayerPost(rl, currentTexture);
      }
      if(this->globalPostStack && !this->globalPostStack->isEmpty())
      {
        this->postProcessGlobal();
      }
      this->drawToBackBuffer();
    }
  }

  void Renderer::renderWithoutLayerPost(const RenderList& rl, std::shared_ptr<Texture>& currentTexture)
  {
    for(const auto& entry : rl.list)
    {
      if(!currentTexture)
      {
        if(entry.textureComp && entry.textureComp->texture)
        {
          currentTexture = entry.textureComp->texture;
          currentTexture->use();
        }
      }
      else
      {
        if(entry.textureComp && entry.textureComp->texture && entry.textureComp->texture->handle != currentTexture->handle)
        {
          currentTexture = entry.textureComp->texture;
          currentTexture->use();
        }
      }
      this->drawRenderable(entry);
    }
  }
  
  void Renderer::renderWithLayerPost(RenderList& rl, std::shared_ptr<Texture>& currentTexture)
  {
    this->scratch.use();
    this->clearCurrentFramebuffer();
    this->pingPong();
    bool bind = false;
    size_t prevLayer = 0;

    if(rl.front().layerComp)
    {
      prevLayer = rl.front().layerComp->layer;
    }
    
    for(size_t i = 0; i < rl.size(); i++)
    {
      auto& entry = rl[i];

      if(!currentTexture)
      {
        if(entry.textureComp && entry.textureComp->texture)
        {
          bind = true;
         currentTexture = entry.textureComp->texture;
        }
      }
      else
      {
        if(entry.textureComp && entry.textureComp->texture && entry.textureComp->texture->handle != currentTexture->handle)
        {
          bind = true;
          currentTexture = entry.textureComp->texture;
        }
      }
      
      if(i == 0)
      {
        if(bind && currentTexture)
        {
          currentTexture->use();
        }
        
        this->drawRenderable(entry);
      }
      else if(i == rl.size() - 1)
      {
        if(entry.layerComp && entry.layerComp->layer != prevLayer)
        {
          this->postProcessLayer(prevLayer);
          this->drawToScratch();
          this->pingPong();
          if(currentTexture)
          {
            currentTexture->use();
          }
        }
        
        if(bind && currentTexture)
        {
          currentTexture->use();
        }
        
        this->drawRenderable(entry);
        if(entry.layerComp)
        {
          this->postProcessLayer(entry.layerComp->layer);
        }
        this->drawToScratch();
      }
      else
      {
        if(entry.layerComp && entry.layerComp->layer != prevLayer)
        {
          this->postProcessLayer(prevLayer);
          this->drawToScratch();
          this->pingPong();
          if(currentTexture)
          {
            currentTexture->use();
          }
        }
        
        if(bind && currentTexture)
        {
          currentTexture->use();
        }
        
        this->drawRenderable(entry);
      }

      if(entry.layerComp)
      {
        prevLayer = entry.layerComp->layer;
      }
      
      bind = false;
    }
    
    this->scratchToPingPong();
  }

  void Renderer::drawRenderable(const Renderable& entry)
  {
    if(isTemplate(entry, OBJECT_RENDERABLE_TEMPLATE) && entry.meshComp->mesh && entry.fragVertShaderComp->shader) //Standard object rendered with a frag/vert shader
    {
      this->model = modelMatrix(entry.transformComp->pos, entry.transformComp->rotation, entry.transformComp->scale);
      this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
      entry.fragVertShaderComp->shader->use();
      entry.fragVertShaderComp->shader->setUniform("mvp", this->mvp);
      entry.fragVertShaderComp->shader->sendUniforms();
      entry.meshComp->mesh->use();
      if(entry.meshComp->mesh->isIndexed())
      {
        this->drawIndexed(entry.meshComp->mesh->drawMode, entry.meshComp->mesh->numIndices);
      }
      else
      {
        this->draw(entry.meshComp->mesh->drawMode, entry.meshComp->mesh->numVerts);
      }
    }
    else if(isTemplate(entry, COMPUTE_RENDERABLE_TEMPLATE)) //Compute image generation
    {
      //TODO finish compute shader path
      for(const auto& [binding, image] : entry.computeShaderComp->imageBindings)
      {
        this->bindImage(binding, image->handle, entry.computeShaderComp->ioMode, entry.computeShaderComp->glColorFormat);
      }
      entry.computeShaderComp->shader->use();
      entry.computeShaderComp->shader->sendUniforms();
      this->startComputeShader(this->contextSize);
    }
    else if(isTemplate(entry, TEXT_RENDERABLE_TEMPLATE) && entry.meshComp->mesh && entry.fragVertShaderComp->shader) //Text object rendered with a frag/vert shader
    {
      const GLRFilterMode prevMin = this->filterModeMin;
      const GLRFilterMode prevMag = this->filterModeMag;
      this->setFilterMode(GLRFilterMode::BILINEAR, GLRFilterMode::BILINEAR);
      
      //TODO batch render text quads
      Mesh textMesh;
      textMesh.setPositionDimensions(GLRDimensions::TWO_DIMENSIONAL);
      textMesh.bufferType = GLRBufferType::SEPARATE;
      textMesh.drawType = GLRDrawType::STATIC_DRAW;
      textMesh.drawMode = GLRDrawMode::TRIS;
      for(const auto& charInfo : entry.textComp->characterInfo)
      {
        //TODO FIXME character info needs a position and scale, then apply that info to the quad position for batching
        constexpr static std::array quadIndices{0u, 1u, 2u, 2u, 3u, 0u};
        constexpr static std::array quadVerts{0.f, 0.f,  1.f, 0.f,  1.f, 1.f,  1.f, 1.f,  0.f, 1.f,  0.f, 0.f}; //ll origin
        const auto& [ul, ll, ur, lr] = charInfo.atlasUVs;
        const std::array quadUVs{lr.x(), lr.y(), ll.x(), ll.y(), ur.x(), ur.y(), ul.x(), ul.y()};
        textMesh.addPositions(quadVerts.data(), quadVerts.size())->addUVs(quadUVs.data(), quadUVs.size())->addIndices(quadIndices.data(), quadIndices.size());
      }
      textMesh.finalize();
      textMesh.use();
      this->model = modelMatrix(entry.transformComp->pos, entry.transformComp->rotation, entry.transformComp->scale);
      this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
      
      entry.fragVertShaderComp->shader->use();
      entry.fragVertShaderComp->shader->setUniform("mvp", this->mvp);
      entry.fragVertShaderComp->shader->sendUniforms();

      if(textMesh.isIndexed())
      {
        this->drawIndexed(textMesh.drawMode, textMesh.numIndices);
      }
      else
      {
        this->draw(textMesh.drawMode, textMesh.numVerts);
      }
      this->setFilterMode(prevMin, prevMag);
    }
  }
  
  void Renderer::drawToScratch() const
  {
    this->fullscreenQuad->use();
    this->scratch.use();
    this->shaderTransfer->use();
    this->curFBO.get() ? this->fboA.bindAttachment(GLRAttachment::COLOR, GLRAttachmentType::TEXTURE, 0) : this->fboB.bindAttachment(GLRAttachment::COLOR, GLRAttachmentType::TEXTURE, 0);
    this->draw(GLRDrawMode::TRI_STRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::scratchToPingPong()
  {
    this->fullscreenQuad->use();
    this->pingPong();
    this->shaderTransfer->use();
    this->scratch.bindAttachment(GLRAttachment::COLOR, GLRAttachmentType::TEXTURE, 0);
    this->draw(GLRDrawMode::TRI_STRIPS, this->fullscreenQuad->numVerts);
  }

  void Renderer::postProcessLayer(const uint64_t layer)
  {
    for(const auto& stage: this->layerPostStack[layer]->getPasses())
    {
      if(stage.enabled)
      {
        this->pingPong();
        stage.process(this->curFBO.get() ? this->fboA : this->fboB, this->curFBO.get() ? this->fboB : this->fboA, stage.userData);
      }
    }
  }
  
  void Renderer::postProcessGlobal()
  {
    for(const auto& stage: this->globalPostStack->getPasses())
    {
      if(stage.enabled)
      {
        this->pingPong();
        stage.process(this->curFBO.get() ? this->fboA : this->fboB, this->curFBO.get() ? this->fboB : this->fboA, stage.userData);
      }
    }
  }

  void Renderer::pingPong()
  {
    this->curFBO.swap() ? this->fboA.use() : this->fboB.use();
    this->clearCurrentFramebuffer();
  }
}

namespace glr
{
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
  

  PipelineRenderer::PipelineRenderer(const GLLoadFunc loadFunc, const uint32_t contextWidth, const uint32_t contextHeight)
  {
    gladLoadGL(loadFunc);
    this->viewport = {contextWidth, contextHeight};
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    glViewport(0, 0, (GLsizei)contextWidth, (GLsizei)contextHeight);
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->maxTextureUnitsPerStage);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &this->maxTextureUnits);
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
          printf("Set model\n");
          this->model = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_ORTHO_PROJECTION_MATRIX:
        {
          printf("Set ortho projection\n");
          this->projection = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_PERSP_PROJECTION_MATRIX:
        {
          printf("Set perspective projection\n");
          this->projection = matrixCallback();
          break;
        }
        case Pipeline::OpCode::SET_VIEW_MATRIX:
        {
          printf("Set view\n");
          this->view = matrixCallback();
          break;
        }
        case Pipeline::OpCode::CALCULATE_MVP:
        {
          printf("Calc MVP\n");
          this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_COLOR:
        {
          printf("Set color clear value\n");
          const auto& c = data.varVec4f;
          glClearColor(c.r(), c.g(), c.b(), c.a());
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_DEPTH:
        {
          printf("Set depth clear value\n");
          glClearDepth(data.varF);
          break;
        }
        case Pipeline::OpCode::SET_CLEAR_STENCIL:
        {
          printf("Set stencil clear value\n");
          glClearStencil(data.varI32);
          break;
        }
        case Pipeline::OpCode::CLEAR:
        {
          printf("Clear\n");
          glClear(enumA | enumB | enumC);
          break;
        }

        case Pipeline::OpCode::USE_ATTACH:
        {
          printf("Use FBO attachment %zu, 0x%04x, 0x%04x, %u\n", id, enumA, enumB, target);
          asset_repo::fboBindAttachment(id, (GLRAttachment)enumA, (GLRAttachmentType)enumB, target);
          break;
        }
        case Pipeline::OpCode::USE_TEX:
        {
          auto& tex = curPipeline.currentTexture;
          if(target >= tex.size() || tex.at(target) == id)
          {
            printf("Use texture: already bound\n");
            continue;
          }
          printf("Use texture\n");
          tex.at(target) = id;
          asset_repo::textureSetBindingTarget(id, target);
          asset_repo::textureUse(id);
          break;
        }
        case Pipeline::OpCode::USE_IMG: //TODO support layered images/levels
        {
          if(target >= curPipeline.currentTexture.size() || curPipeline.currentTexture.at(target) == id)
          {
            printf("Use image: already bound\n");
            continue;
          }
          printf("Use image\n");
          curPipeline.currentTexture.at(target) = id;
          asset_repo::textureUseAsImage(id, target, (GLRIOMode)enumA, (GLRColorFormat)enumB);
          break;
        }
        case Pipeline::OpCode::USE_SHADER:
        {
          if(curPipeline.currentShader == id)
          {
            printf("Use shader: already bound\n");
            continue;
          }
          printf("Use shader\n");
          curPipeline.currentShader = id;
          asset_repo::shaderUse(id);
          break;
        }
        case Pipeline::OpCode::USE_MESH:
        {
          if(curPipeline.currentMesh == id)
          {
            printf("Use mesh: already bound\n");
            continue;
          }
          printf("Use mesh\n");
          curPipeline.currentMesh = id;
          asset_repo::meshUse(id);
          break;
        }
        case Pipeline::OpCode::USE_BACKBUFFER:
        {
          printf("Use backbuffer\n");
          asset_repo::fboUse(0);
          break;
        }
        case Pipeline::OpCode::USE_FBO:
        {
          if(curPipeline.currentFramebuffer == id)
          {
            printf("Use framebuffer: already bound\n");
            continue;
          }
          printf("Use framebuffer\n");
          curPipeline.currentFramebuffer = id;
          asset_repo::fboUse(id);
          break;
        }
        case Pipeline::OpCode::USE_PIPELINE:
        {
          if(curPipeline.currentShaderPipeline == id)
          {
            printf("Use shader pipeline: already bound\n");
            continue;
          }
          printf("Use shader pipeline\n");
          curPipeline.currentShaderPipeline = id;
          asset_repo::shaderPipelineUse(id);
          break;
        }

        case Pipeline::OpCode::SET_UNI_F:
        {
          printf("Set float uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varF);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U8:
        {
          printf("Set u8 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU8);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I8:
        {
          printf("Set i8 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI8);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U16:
        {
          printf("Set u16 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU16);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I16:
        {
          printf("Set i16 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI16);
          break;
        }
        case Pipeline::OpCode::SET_UNI_U32:
        {
          printf("Set u32 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varU32);
          break;
        }
        case Pipeline::OpCode::SET_UNI_I32:
        {
          printf("Set i32 uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varI32);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC2U:
        {
          printf("Set vec2u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC2I:
        {
          printf("Set vec2i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC2F:
        {
          printf("Set vec2f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec2f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC3U:
        {
          printf("Set vec3u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC3I:
        {
          printf("Set vec3i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC3F:
        {
          printf("Set vec3f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec3f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_VEC4U:
        {
          printf("Set vec4u uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4u);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC4I:
        {
          printf("Set vec4i uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4i);
          break;
        }
        case Pipeline::OpCode::SET_UNI_VEC4F:
        {
          printf("Set vec4f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varVec4f);
          break;
        }

        case Pipeline::OpCode::SET_UNI_MAT3F:
        {
          printf("Set mat3x3f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varMat3f);
          break;
        }
        case Pipeline::OpCode::SET_UNI_MAT4F:
        {
          printf("Set mat4x4f uniform\n");
          asset_repo::shaderSetUniform(id, name, data.varMat4f);
          break;
        }
        case Pipeline::OpCode::SET_UNI_MVP:
        {
          printf("Set MVP uniform\n");
          asset_repo::shaderSetUniform(id, name, this->mvp);
          break;
        }

        case Pipeline::OpCode::SEND_UNIFORMS:
        {
          printf("Send uniforms\n");
          asset_repo::shaderSendUniforms(id);
          break;
        }

        case Pipeline::OpCode::DRAW:
        {
          printf("Draw %zu vertices with mode 0x%04x\n", data.varU64, enumA);
          glDrawArrays((GLenum)enumA, 0, data.varU64);
          break;
        }
        case Pipeline::OpCode::DRAW_INDEXED:
        {
          printf("Draw %zu indices with draw mode 0x%04x, index buffer format 0x%04x\n", data.varU64, enumA, enumB);
          glDrawElements((GLenum)enumA, data.varU64, (int32_t)enumB, nullptr);
          break;
        }
        case Pipeline::OpCode::DISPATCH_COMPUTE:
        {
          printf("Dispatch compute shader\n");
          glDispatchCompute((uint32_t)(std::ceil((float)(this->contextSizeX) / (float)curPipeline.workSizeX)), (uint32_t)(std::ceil((float)(this->contextSizeY) / (float)curPipeline.workSizeY)), 1);
          break;
        }

        case Pipeline::OpCode::SET_FILTER_MODE:
        {
          printf("Set filter modes\n");
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
          printf("Set blend\n");
          data.varBool ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
          break;
        }
        case Pipeline::OpCode::SET_BLEND_MODE:
        {
          printf("Set blend modes\n");
          glBlendFunc((int32_t)enumA, (int32_t)enumB);
          break;
        }
        case Pipeline::OpCode::SET_DEPTH_TEST:
        {
          printf("Set depth testing\n");
          data.varBool ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
          break;
        }
        case Pipeline::OpCode::SET_CULL_BACKFACE:
        {
          printf("Set backface culling\n");
          data.varBool ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
          break;
        }
        case Pipeline::OpCode::SET_SCISSOR_TEST:
        {
          printf("Set scissor testing\n");
          data.varBool ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
          break;
        }
          
        case Pipeline::OpCode::INVALID:
        default: printf("Invalid opcode\n"); break;
      }
    }
    printf("\n\n");
  }
}
