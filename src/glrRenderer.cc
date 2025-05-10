#include "glrender/glrRenderer.hh"

#include <glad/gl.hh>

namespace glr
{
  uint32_t Renderer::WORK_SIZE_X = 40;
  uint32_t Renderer::WORK_SIZE_Y = 20;

/// ===Data===========================================================================///
  
  std::string transferFrag =
R"(#version 450

in vec2 uv;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
  fragColor = texture(tex, uv);
})";
  
  std::string transferVert =
R"(#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv_in;
out vec2 uv;

void main()
{
  uv = uv_in;
  gl_Position = vec4(pos, 1.0);
})";
  
  std::array fullscreenQuadVerts{1.f, -1.f, 0.f, 1.f, 1.f, 0.f, -1.f, -1.f, 0.f, -1.f, 1.f, 0.f};
  std::array fullscreenQuadUVs{1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f};
  
  void glDebug(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, const GLsizei messageLength, const GLchar* message, const void* userParam)
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

/// ===Renderer========================================================================================================================================///
  
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
    this->fboPool = FramebufferPool(2, contextWidth, contextHeight);
    this->fboA = Framebuffer(contextWidth, contextHeight, std::initializer_list{GLAttachment::COLOR_TEXTURE, GLAttachment::ALPHA_TEXTURE}, "Ping");
    this->fboB = Framebuffer(contextWidth, contextHeight, std::initializer_list{GLAttachment::COLOR_TEXTURE, GLAttachment::ALPHA_TEXTURE}, "Pong");
    this->scratch = Framebuffer(contextWidth, contextHeight, std::initializer_list{GLAttachment::COLOR_TEXTURE}, "Scratch");
    this->fullscreenQuad = std::make_unique<Mesh>();
    this->fullscreenQuad->addPositions(fullscreenQuadVerts.data(), fullscreenQuadVerts.size())->addUVs(fullscreenQuadUVs.data(), fullscreenQuadUVs.size());
    this->shaderTransfer = std::make_unique<Shader>("Transfer Shader", transferVert, transferFrag);
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, (GLsizei)contextWidth, (GLsizei)contextHeight);
  }
  
  Renderer::~Renderer()
  {
    this->fboA.reset();
    this->fboB.reset();
    this->scratch.reset();
    this->shaderTransfer.reset();
    this->globalPostStack.reset();
    this->layerPostStack.clear();
  }
  
  void Renderer::onContextResize(const uint32_t width, const uint32_t height)
  {
    this->contextSize = {width, height};
    this->useBackBuffer();
    glViewport(0, 0, (int32_t)width, (int32_t)height);
    this->fboPool.onResize(width, height);
  }
  
  void Renderer::setGlobalPostStack(std::shared_ptr<PostStack> stack)
  {
    this->globalPostStack = std::move(stack);
  }
  
  void Renderer::setLayerPostStack(const uint64_t layer, std::shared_ptr<PostStack> stack)
  {
    this->layerPostStack[layer] = std::move(stack);
  }
  
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
  
  void Renderer::setFilterMode(const FilterMode min, const FilterMode mag)
  {
    this->filterModeMin = min;
    this->filterModeMag = mag;
    
    switch(min)
    {
      case NEAREST:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
      }
      
      case BILINEAR:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
      }
      
      case TRILINEAR:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
      }
    }
    
    switch(mag)
    {
      case NEAREST:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
      }
      
      case BILINEAR:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      }
      
      case TRILINEAR:
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
      }
    }
  }
  
  void Renderer::draw(const GLDrawMode mode, const size_t numElements) const
  {
    glDrawArrays((GLenum)mode, 0, (GLsizei)numElements);
  }
  
  void Renderer::pingPong()
  {
    this->curFBO.swap() ? this->fboA.use() : this->fboB.use();
    this->clearCurrentFramebuffer();
  }
  
  void Renderer::drawToBackBuffer() const
  {
    this->fullscreenQuad->use();
    this->useBackBuffer();
    this->clearCurrentFramebuffer();
    this->shaderTransfer->use();
    this->curFBO.get() ? this->fboA.bind(GLAttachment::COLOR_TEXTURE, 0) : this->fboB.bind(GLAttachment::COLOR_TEXTURE, 0);
    this->draw(GLDrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::drawToScratch() const
  {
    this->fullscreenQuad->use();
    this->scratch.use();
    this->shaderTransfer->use();
    this->curFBO.get() ? this->fboA.bind(GLAttachment::COLOR_TEXTURE, 0) : this->fboB.bind(GLAttachment::COLOR_TEXTURE, 0);
    this->draw(GLDrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::scratchToPingPong()
  {
    this->fullscreenQuad->use();
    this->pingPong();
    this->shaderTransfer->use();
    this->scratch.bind(GLAttachment::COLOR_TEXTURE, 0);
    this->draw(GLDrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::bindImage(const uint32_t target, const uint32_t handle, const IOMode mode, const GLColorFormat format) const
  {
    glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
  }
  
  void Renderer::startComputeShader(const vec2<uint32_t>& contextSize, const vec2<uint32_t>& workSize) const
  {
    glDispatchCompute((uint32_t)(std::ceil((float)(contextSize.x()) / (float)workSize.x())), (uint32_t)(std::ceil((float)(contextSize.y()) / (float)workSize.y())), 1);
  }
  
  //==Rendering==================================================================================================================================================================================================
  void Renderer::render(RenderList renderList, const mat4x4<float>& viewMat, const mat4x4<float>& projectionMat)
  {
    RenderList rl = std::move(renderList);
    if(rl.empty())
    {
      return;
    }
    
    this->view = viewMat;
    this->projection = projectionMat;

    std::shared_ptr<Texture> curTexture = nullptr;
    if(rl.front().textureComp && rl.front().textureComp->texture)
    {
      curTexture = rl.front().textureComp->texture;
      curTexture->use(0);
    }
    
    this->layerPostStack.empty() ? this->renderWithoutPost(rl, curTexture) : this->renderWithPost(rl, curTexture);
    
    if(this->globalPostStack && !this->globalPostStack->isEmpty())
    {
      this->postProcessGlobal();
    }
    
    this->drawToBackBuffer();
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

  void Renderer::renderWithoutPost(const RenderList& renderList, std::shared_ptr<Texture>& curTexture)
  {
    this->pingPong();
    
    for(const auto& entry : renderList.list)
    {
      if(!curTexture)
      {
        if(entry.textureComp && entry.textureComp->texture)
        {
          curTexture = entry.textureComp->texture;
          curTexture->use(0);
        }
      }
      else
      {
        if(entry.textureComp && entry.textureComp->texture && entry.textureComp->texture->handle != curTexture->handle)
        {
          curTexture = entry.textureComp->texture;
          curTexture->use(0);
        }
      }
      
      this->drawRenderable(entry);
    }
  }
  
  void Renderer::renderWithPost(RenderList& renderList, std::shared_ptr<Texture>& curTexture)
  {
    this->scratch.use();
    this->clearCurrentFramebuffer();
    this->pingPong();
    bool bind = false;
    size_t prevLayer = 0;

    if(renderList.front().layerComp)
    {
      prevLayer = renderList.front().layerComp->layer;
    }
    
    for(size_t i = 0; i < renderList.size(); i++)
    {
      auto& entry = renderList[i];

      if(!curTexture)
      {
        if(entry.textureComp && entry.textureComp->texture)
        {
          bind = true;
          curTexture = entry.textureComp->texture;
        }
      }
      else
      {
        if(entry.textureComp && entry.textureComp->texture && entry.textureComp->texture->handle != curTexture->handle)
        {
          bind = true;
          curTexture = entry.textureComp->texture;
        }
      }
      
      if(i == 0)
      {
        if(bind && curTexture)
        {
          curTexture->use(0);
        }
        
        this->drawRenderable(entry);
      }
      else if(i == renderList.size() - 1)
      {
        if(entry.layerComp && entry.layerComp->layer != prevLayer)
        {
          this->postProcessLayer(prevLayer);
          this->drawToScratch();
          this->pingPong();
          if(curTexture)
          {
            curTexture->use(0);
          }
        }
        
        if(bind && curTexture)
        {
          curTexture->use(0);
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
          if(curTexture)
          {
            curTexture->use(0);
          }
      }
        
        if(bind && curTexture)
        {
          curTexture->use(0);
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
        this->draw(entry.meshComp->mesh->drawMode, entry.meshComp->mesh->numVerts);
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
      const FilterMode prevMin = this->filterModeMin;
      const FilterMode prevMag = this->filterModeMag;
      this->setFilterMode(BILINEAR, BILINEAR);
      
      //TODO batch render text quads
      Mesh textMesh;
      for(const auto& charInfo : entry.textComp->characterInfo)
      {
        //TODO FIXME apply entry's position and scale to the quad's positions
        constexpr std::array quadVerts{1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f}; //Lower left origin
        const auto& [ul, ll, ur, lr] = charInfo.atlasUVs;
        const std::array quadUVs{lr.x(), lr.y(), ll.x(), ll.y(), ur.x(), ur.y(), ul.x(), ul.y()};
        textMesh.addPositions(quadVerts.data(), quadVerts.size())->addUVs(quadUVs.data(), quadUVs.size())->use();
      }
      this->model = modelMatrix(entry.transformComp->pos, entry.transformComp->rotation, entry.transformComp->scale);
      this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
      
      entry.fragVertShaderComp->shader->use();
      entry.fragVertShaderComp->shader->setUniform("mvp", this->mvp);
      //entry.fragVertShaderComp->shader->setUniform(charInfo.colorUniformLocation, charInfo.color.asRGBAf());
      entry.fragVertShaderComp->shader->sendUniforms();
      
      this->draw(textMesh.drawMode, textMesh.numVerts);
      this->setFilterMode(prevMin, prevMag);
    }
  }
}
