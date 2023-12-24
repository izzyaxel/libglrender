#include "glrFramebuffer.hh"

#include <algorithm>
#include <glad/gl.hh>

namespace glr
{
  Framebuffer::Framebuffer(uint32_t width, uint32_t height, const std::initializer_list<Attachment> &options, const std::string &name)
  {
    this->width = width;
    this->height = height;
    for(auto const &option: options)
    {
      switch(option)
      {
        case Attachment::COLOR:
          this->hasColor = true;
          break;
        case Attachment::ALPHA:
          this->hasAlpha = true;
          break;
        case Attachment::DEPTH:
          this->hasDepth = true;
          break;
        case Attachment::STENCIL:
          this->hasStencil = true;
          break;
        default:
          break;
      }
    }
    this->name = name;
    this->init = true;
    this->createFBO();
  }
  
  Framebuffer::~Framebuffer()
  {
    this->clearFBO();
  }
  
  Framebuffer::Framebuffer(Framebuffer &&other) noexcept
  {
    this->handle = other.handle;
    other.handle = std::numeric_limits<uint32_t>::max();
    
    this->colorHandle = other.colorHandle;
    other.colorHandle = std::numeric_limits<uint32_t>::max();
    
    this->depthHandle = other.depthHandle;
    other.depthHandle = std::numeric_limits<uint32_t>::max();
    
    this->stencilHandle = other.stencilHandle;
    other.stencilHandle = std::numeric_limits<uint32_t>::max();
    
    this->width = other.width;
    other.width = 0;
    
    this->height = other.height;
    other.height = 0;
    
    this->hasColor = other.hasColor;
    other.hasColor = false;
    
    this->hasDepth = other.hasDepth;
    other.hasDepth = false;
    
    this->hasAlpha = other.hasAlpha;
    other.hasAlpha = false;
    
    this->hasStencil = other.hasStencil;
    other.hasStencil = false;
    
    this->name = other.name;
    other.name = "";
    
    this->init = true;
    other.init = false;
  }
  
  Framebuffer &Framebuffer::operator =(Framebuffer &&other) noexcept
  {
    this->handle = other.handle;
    other.handle = std::numeric_limits<uint32_t>::max();
    
    this->colorHandle = other.colorHandle;
    other.colorHandle = std::numeric_limits<uint32_t>::max();
    
    this->depthHandle = other.depthHandle;
    other.depthHandle = std::numeric_limits<uint32_t>::max();
    
    this->stencilHandle = other.stencilHandle;
    other.stencilHandle = std::numeric_limits<uint32_t>::max();
    
    this->width = other.width;
    other.width = 0;
    
    this->height = other.height;
    other.height = 0;
    
    this->hasColor = other.hasColor;
    other.hasColor = false;
    
    this->hasDepth = other.hasDepth;
    other.hasDepth = false;
    
    this->hasAlpha = other.hasAlpha;
    other.hasAlpha = false;
    
    this->hasStencil = other.hasStencil;
    other.hasStencil = false;
    
    this->name = other.name;
    other.name = "";
    
    this->init = true;
    other.init = false;
    
    return *this;
  }
  
  bool Framebuffer::exists() const
  {
    return this->init;
  }
  
  void Framebuffer::reset()
  {
    this->handle = std::numeric_limits<uint32_t>::max();
    this->colorHandle = std::numeric_limits<uint32_t>::max();
    this->depthHandle = std::numeric_limits<uint32_t>::max();
    this->stencilHandle = std::numeric_limits<uint32_t>::max();
    this->width = std::numeric_limits<uint32_t>::max();
    this->height = std::numeric_limits<uint32_t>::max();
    this->hasColor = false;
    this->hasDepth = false;
    this->hasAlpha = false;
    this->hasStencil = false;
    this->name = "";
    this->init = false;
  }
  
  void Framebuffer::use() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, this->handle);
  }
  
  void Framebuffer::bind(Attachment type, uint32_t target) const
  {
    switch(type)
    {
      case Attachment::COLOR:
        glBindTextureUnit(target, this->colorHandle);
        break;
      case Attachment::DEPTH:
        glBindTextureUnit(target, this->depthHandle);
        break;
      case Attachment::STENCIL:
        glBindTextureUnit(target, this->stencilHandle);
        break;
      default:
        break;
    }
  }
  
  void Framebuffer::regenerate(uint32_t width, uint32_t height)
  {
    this->width = width;
    this->height = height;
    this->clearFBO();
    this->createFBO();
  }
  
  void Framebuffer::createFBO()
  {
    glCreateFramebuffers(1, &this->handle);
    this->use();
    glViewport(0, 0, (GLsizei) this->width, (GLsizei) this->height);
    glScissor(0, 0, (GLsizei) this->width, (GLsizei) this->height);
    if(this->hasColor) glCreateTextures(GL_TEXTURE_2D, 1, &this->colorHandle);
    if(this->hasDepth) glCreateTextures(GL_TEXTURE_2D, 1, &this->depthHandle);
    glTextureStorage2D(this->colorHandle, 1, this->hasAlpha ? GL_RGBA32F : GL_RGB32F, (GLsizei) this->width, (GLsizei) this->height);
    glNamedFramebufferTexture(this->handle, GL_COLOR_ATTACHMENT0, this->colorHandle, 0);
    if(this->hasDepth)
    {
      glTextureStorage2D(this->depthHandle, 1, GL_DEPTH_COMPONENT32F, (GLsizei) this->width, (GLsizei) this->height);
      glNamedFramebufferTexture(this->handle, GL_DEPTH_ATTACHMENT, this->depthHandle, 0);
    }
    std::vector<GLenum> drawBuffers;
    drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
    if(this->hasDepth) drawBuffers.emplace_back(GL_COLOR_ATTACHMENT1);
    glNamedFramebufferDrawBuffers(this->handle, static_cast<int32_t>(drawBuffers.size()), drawBuffers.data());
    GLenum error = glCheckNamedFramebufferStatus(this->handle, GL_FRAMEBUFFER);
    if(error != GL_FRAMEBUFFER_COMPLETE)
    {
      std::string er = "Framebuffer Creation Error: ";
      switch(error)
      {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          er += "incomplete attachment";
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
          er += "incomplete dimensions";
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          er += "missing attachment";
          break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
          er += "Framebuffers are not supported";
          break;
        default:
          break;
      }
      printf("%s\n", er.c_str());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void Framebuffer::clearFBO()
  {
    glDeleteFramebuffers(1, &this->handle);
    glDeleteTextures(1, &this->colorHandle);
    glDeleteTextures(1, &this->depthHandle);
  }
  
  FramebufferPool::FramebufferPool(size_t alloc, uint32_t width, uint32_t height)
  {
    this->pool.resize(alloc);
    for(size_t i = 0; i < alloc; i++)
    {
      this->pool[i] = Framebuffer(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(i));
    }
    this->init = true;
  }
  
  FramebufferPool::FramebufferPool(FramebufferPool &&other) noexcept
  {
    this->pool = std::move(other.pool);
    other.pool.clear();
    
    this->init = true;
    other.init = false;
  }
  
  FramebufferPool& FramebufferPool::operator=(FramebufferPool &&other) noexcept
  {
    this->pool = std::move(other.pool);
    other.pool.clear();
    
    this->init = true;
    other.init = false;
    
    return *this;
  }
  
  bool FramebufferPool::exists() const
  {
    return this->init;
  }
  
  void FramebufferPool::reset()
  {
    this->pool.clear();
    this->init = false;
  }
  
  Framebuffer& FramebufferPool::getNextAvailableFBO(uint32_t width, uint32_t height)
  {
    Framebuffer out;
    for(auto &fbo: this->pool)
    {
      if(fbo.exists())
      {
        if(fbo.width != width || fbo.height != height)
        {
          fbo = Framebuffer(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(this->pool.size() + 1));
        }
        fbo.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return fbo;
      }
    }
    this->pool.emplace_back(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(this->pool.size() + 1));
    this->pool.back().use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return this->pool.back();
  }
  
  void FramebufferPool::onResize(uint32_t width, uint32_t height)
  {
    for(auto &fbo: this->pool)
    {
      fbo.regenerate(width, height);
    }
  }
}
