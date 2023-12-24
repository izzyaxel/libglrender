#include "glrRenderer.hh"

#include <glad/gl.hh>

namespace glr
{
  uint32_t Renderer::WORKSIZEX = 40;
  uint32_t Renderer::WORKSIZEY = 20;

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
  
  std::string textFrag =
    R"(#version 450

in vec2 uv;
uniform vec4 inputColor = vec4(1, 1, 1, 1);
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
	fragColor = vec4(inputColor.rgb, texture(tex, uv).r * inputColor.w);
})";
  
  std::string textVert =
    R"(#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv_in;
out vec2 uv;
uniform mat4 mvp;

void main()
{
	uv = uv_in;
	gl_Position = mvp * vec4(pos, 1.0);
})";
  
  std::vector<float> fullscreenQuadVerts{1, -1, 0, 1, 1, 0, -1, -1, 0, -1, 1, 0};
  std::vector<float> fullscreenQuadUVs{1, 0, 1, 1, 0, 0, 0, 1};
/// ===Data===========================================================================///
  
  
  void glDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei messageLength, GLchar const *message, void const *userParam)
  {
    std::string sev;
    std::string ty;
    std::string src;
    
    switch(source)
    {
      case GL_DEBUG_SOURCE_API:
        src = "Source: OpenGL API";
        break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        src = "Source: Window-system API";
        break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        src = "Source: Shader Compiler";
        break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        src = "Source: Third-party Application";
        break;
      case GL_DEBUG_SOURCE_APPLICATION:
        src = "Source: User's Application";
        break;
      case GL_DEBUG_SOURCE_OTHER:
        src = "Source: Other";
        break;
      default:
        break;
    }
    switch(severity)
    {
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        sev = "Severity: NOTIFICATION";
        break;
      case GL_DEBUG_SEVERITY_LOW:
        sev = "Severity: LOW";
        break;
      case GL_DEBUG_SEVERITY_MEDIUM:
        sev = "Severity: MEDIUM";
        break;
      case GL_DEBUG_SEVERITY_HIGH:
        sev = "Severity: HIGH";
        break;
      default:
        break;
    }
    switch(type)
    {
      case GL_DEBUG_TYPE_ERROR:
        ty = "Type: Error";
        break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        ty = "Type: Deprecated Behavior";
        break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        ty = "Type: Undefined Behavior";
        break;
      case GL_DEBUG_TYPE_PORTABILITY:
        ty = "Type: Portability";
        break;
      case GL_DEBUG_TYPE_PERFORMANCE:
        ty = "Type: Performance";
        break;
      case GL_DEBUG_TYPE_MARKER:
        ty = "Type: Command Stream Annotation";
        break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
        ty = "Type: Group Pushing";
        break;
      case GL_DEBUG_TYPE_POP_GROUP:
        ty = "Type: Group Popping";
        break;
      case GL_DEBUG_TYPE_OTHER:
        ty = "Type: Other";
        break;
      default:
        break;
    }
    printf("An OpenGL error occured: [%s] %s, ID: %u, %s, Message: %s\n", src.c_str(), sev.c_str(), id, ty.c_str(), message);
  }

/// ===Renderer========================================================================================================================================///
  Renderer::Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight)
  {
    gladLoadGL(loadFunc);
    this->fboPool = std::make_unique<FramebufferPool>(2, contextWidth, contextHeight);
    this->fboA = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA}, "Ping");
    this->fboB = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA}, "Pong");
    this->scratch = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR}, "Scratch");
    this->fullscreenQuad = std::make_unique<Mesh>(fullscreenQuadVerts, fullscreenQuadUVs);
    this->shaderTransfer = std::make_unique<Shader>("Transfer Shader", transferVert, transferFrag);
    this->shaderText = std::make_unique<Shader>("Text Shader", textVert, textFrag);
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebug, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, (GLsizei) contextWidth, (GLsizei) contextHeight);
  }
  
  Renderer::~Renderer()
  {
    this->fboA.reset();
    this->fboB.reset();
    this->scratch.reset();
    this->fullscreenQuad.reset();
    this->shaderTransfer.reset();
    this->globalPostStack.reset();
    this->layerPostStack.clear();
  }
  
  void Renderer::onContextResize(uint32_t width, uint32_t height)
  {
    this->useBackBuffer();
    glViewport(0, 0, (int) width, (int) height);
    this->fboPool->onResize(width, height);
  }
  
  void Renderer::setGlobalPostStack(std::shared_ptr<PostStack> stack)
  {
    this->globalPostStack = std::move(stack);
  }
  
  void Renderer::setLayerPostStack(uint64_t layer, std::shared_ptr<PostStack> stack)
  {
    this->layerPostStack[layer] = std::move(stack);
  }
  
  void Renderer::useBackBuffer()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void Renderer::render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection)
  {
    if(renderList.empty()) return;
    this->view = view;
    this->projection = projection;
    Texture &curTexture = renderList[0].texture;
    renderList[0].texture.use(0);
    if(this->layerPostStack.empty()) //No postprocessing
    {
      this->pingPong();
      for(auto &entry : renderList.list)
      {
        if(entry.texture.handle != curTexture.handle)
        {
          curTexture = entry.texture;
          entry.texture.use(0);
        }
        this->drawRenderable(entry);
      }
    }
    else //Postprocess
    {
      this->scratch->use();
      this->clearCurrentFramebuffer();
      this->pingPong();
      bool bind = false;
      size_t prevLayer = renderList[0].layer;
      for(size_t i = 0; i < renderList.size(); i++)
      {
        auto &entry = renderList[i];
        if(entry.texture.handle != curTexture.handle)
        {
          bind = true;
          curTexture = entry.texture;
        }
        
        if(i == 0)
        {
          if(bind)
          {
            curTexture.use(0);
          }
          this->drawRenderable(entry);
        }
        else if(i == renderList.size() - 1)
        {
          if(entry.layer != prevLayer)
          {
            this->postProcessLayer(prevLayer);
            this->drawToScratch();
            this->pingPong();
            curTexture.use(0);
          }
          if(bind)
          {
            curTexture.use(0);
          }
          this->drawRenderable(entry);
          this->postProcessLayer(entry.layer);
          this->drawToScratch();
        }
        else
        {
          if(entry.layer != prevLayer)
          {
            this->postProcessLayer(prevLayer);
            this->drawToScratch();
            this->pingPong();
            curTexture.use(0);
          }
          if(bind)
          {
            curTexture.use(0);
          }
          this->drawRenderable(entry);
        }
        prevLayer = entry.layer;
        bind = false;
      }
      this->scratchToPingPong();
    }
    if(this->globalPostStack && !this->globalPostStack->isEmpty())
    {
      this->postProcessGlobal();
    }
    this->drawToBackBuffer();
  }
  
  void Renderer::setClearColor(Color color)
  {
    auto colorF = color.asRGBAf();
    glClearColor(colorF.r(), colorF.g(), colorF.b(), colorF.a());
  }
  
  void Renderer::clearCurrentFramebuffer()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  
  void Renderer::setScissorTest(bool val)
  {
    val ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
  }
  
  void Renderer::setDepthTest(bool val)
  {
    val ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  }
  
  void Renderer::setBlending(bool val)
  {
    val ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  }
  
  void Renderer::setBlendMode(uint32_t src, uint32_t dst)
  {
    glBlendFunc(src, dst);
  }
  
  void Renderer::setCullFace(bool val)
  {
    val ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  }
  
  void Renderer::setFilterMode(FilterMode mode)
  {
    switch(mode)
    {
      case FilterMode::NEAREST:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
      
      case FilterMode::BILINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      
      case FilterMode::TRILINEAR:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
  }
  
  void Renderer::draw(DrawMode mode, size_t numElements)
  {
    glDrawArrays((GLenum) mode, 0, (GLsizei) numElements);
  }
  
  void Renderer::pingPong()
  {
    this->curFBO.swap() ? this->fboA->use() : this->fboB->use();
    this->clearCurrentFramebuffer();
  }
  
  void Renderer::postProcessLayer(uint64_t layer)
  {
    for(auto const &stage: this->layerPostStack[layer]->getPasses())
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
    for(auto const &stage: this->globalPostStack->getPasses())
    {
      if(stage.enabled)
      {
        this->pingPong();
        stage.process(this->curFBO.get() ? this->fboA : this->fboB, this->curFBO.get() ? this->fboB : this->fboA, stage.userData);
      }
    }
  }
  
  void Renderer::drawToBackBuffer()
  {
    this->fullscreenQuad->use();
    this->useBackBuffer();
    this->clearCurrentFramebuffer();
    this->shaderTransfer->use();
    this->curFBO.get() ? this->fboA->bind(Attachment::COLOR, 0) : this->fboB->bind(Attachment::COLOR, 0);
    draw(DrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::drawToScratch()
  {
    this->fullscreenQuad->use();
    this->scratch->use();
    this->shaderTransfer->use();
    this->curFBO.get() ? this->fboA->bind(Attachment::COLOR, 0) : this->fboB->bind(Attachment::COLOR, 0);
    draw(DrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::scratchToPingPong()
  {
    this->fullscreenQuad->use();
    this->pingPong();
    this->shaderTransfer->use();
    this->scratch->bind(Attachment::COLOR, 0);
    draw(DrawMode::TRISTRIPS, this->fullscreenQuad->numVerts);
  }
  
  void Renderer::drawRenderable(Renderable &entry)
  {
    if(entry.characterInfo.character != '\0') //Text rendering
    {
      this->setFilterMode(FilterMode::TRILINEAR);
      quat<float> rotation;
      rotation.fromAxial(vec3<float>{entry.axis}, degToRad<float>((float) entry.rotation));
      vec3<float> posF = vec3<float>{vec2<float>{entry.pos}, 0};
      this->model = modelMatrix(posF, rotation, vec3<float>(vec2<float>{entry.scale}, 1));
      this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
      this->shaderText->use();
      this->shaderText->sendMat4f("mvp", &this->mvp.data[0][0]);
      this->shaderText->sendVec4f("inputColor", entry.characterInfo.color.asRGBAf().data);
      std::array<float, 12> quadVerts{1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0}; //Lower left origin
      std::array<float, 8> quadUVs{entry.characterInfo.atlasUVs.lowerRight.x(), entry.characterInfo.atlasUVs.lowerRight.y(), entry.characterInfo.atlasUVs.lowerLeft.x(), entry.characterInfo.atlasUVs.lowerLeft.y(), entry.characterInfo.atlasUVs.upperRight.x(), entry.characterInfo.atlasUVs.upperRight.y(), entry.characterInfo.atlasUVs.upperLeft.x(), entry.characterInfo.atlasUVs.upperLeft.y()};
      Mesh mesh(quadVerts.data(), quadVerts.size(), quadUVs.data(), quadUVs.size());
      mesh.use();
      draw(DrawMode::TRISTRIPS, mesh.numVerts);
      this->setFilterMode(FilterMode::NEAREST);
    }
    else
    {
      quat<float> rotation;
      rotation.fromAxial(vec3<float>{entry.axis}, degToRad<float>((float) entry.rotation));
      vec3<float> posF = vec3<float>{vec2<float>{entry.pos}, 0};
      this->model = modelMatrix(posF, rotation, vec3<float>(vec2<float>{entry.scale}, 1));
      this->mvp = modelViewProjectionMatrix(this->model, this->view, this->projection);
      entry.shader.use();
      entry.shader.sendMat4f("mvp", &this->mvp.data[0][0]);
      entry.mesh.use();
      draw(DrawMode::TRISTRIPS, entry.mesh.numVerts);
    }
  }
  
  void Renderer::bindImage(uint32_t target, uint32_t const &handle, IOMode mode, GLColorFormat format)
  {
    glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t) mode, (uint32_t) format);
  }
  
  void Renderer::startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize)
  {
    glDispatchCompute((uint32_t) (std::ceil((float) (contextSize.x()) / (float) workSize.x())), (uint32_t) (std::ceil((float) (contextSize.y()) / (float) workSize.y())), 1);
  }
}
