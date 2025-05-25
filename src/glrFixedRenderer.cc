#include "glrender/glrFixedRenderer.hh"

#include <glad/gl.hh>
#include <commons/math/mat4.hh>

namespace glr
{
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

  LoggingCallback cbFixed = nullptr;
  void glFixedDebug(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei messageLength, const GLchar* message, const void* userData)
  {
    if(!cbFixed)
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
    cbFixed(GLRLogType::ERROR, "An OpenGL error occured: [" + sourceStr + "] " + severityStr + ", ID: " + std::to_string(id) + ", " + typeStr + ", Message: " + message + "\n");
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
  
  Renderer::Renderer(const GLLoadFunc loadFunc, const uint32_t contextWidth, const uint32_t contextHeight, const LoggingCallback& callback)
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
    glDebugMessageCallback(glFixedDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, (GLsizei)contextWidth, (GLsizei)contextHeight);

    cbFixed = callback;
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
