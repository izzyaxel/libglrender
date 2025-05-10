#include "GLRender/glrFramebuffer.hh"

#include <algorithm>
#include <glad/gl.hh>

namespace glr
{
  Framebuffer::Framebuffer(const uint32_t width, const uint32_t height, std::initializer_list<GLAttachment> options, const std::string& name)
  {
    this->width = width;
    this->height = height;
    for(auto const &option: options)
    {
      switch(option)
      {
        case GLAttachment::COLOR_TEXTURE:
        {
          this->hasColorTex = true;
          break;
        }
        case GLAttachment::ALPHA_TEXTURE:
        {
          this->hasAlpha = true;
          break;
        }
        case GLAttachment::DEPTH_TEXTURE:
        {
          this->hasDepthTex = true;
          break;
        }
        case GLAttachment::STENCIL_TEXTURE:
        {
          this->hasStencilTex = true;
          break;
        }
        case GLAttachment::COLOR_RENDERBUFFER:
        {
          this->hasColorRB = true;
          break;
        }
        case GLAttachment::DEPTH_RENDERBUFFER:
        {
          this->hasDepthRB = true;
          break;
        }
        case GLAttachment::STENCIL_RENDERBUFFER:
        {
          this->hasStencilRB = true;
          break;
        }
        default: break;
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
  
  Framebuffer::Framebuffer(Framebuffer&& other) noexcept
  {
    this->handle = other.handle;
    other.handle = INVALID_HANDLE;
    
    this->colorHandle = other.colorHandle;
    other.colorHandle = INVALID_HANDLE;
    
    this->depthHandle = other.depthHandle;
    other.depthHandle = INVALID_HANDLE;
    
    this->stencilHandle = other.stencilHandle;
    other.stencilHandle = INVALID_HANDLE;
    
    this->width = other.width;
    other.width = 0;
    
    this->height = other.height;
    other.height = 0;
    
    this->hasColorTex = other.hasColorTex;
    other.hasColorTex = false;
    
    this->hasDepthTex = other.hasDepthTex;
    other.hasDepthTex = false;
    
    this->hasAlpha = other.hasAlpha;
    other.hasAlpha = false;
    
    this->hasStencilTex = other.hasStencilTex;
    other.hasStencilTex = false;
    
    this->name = other.name;
    other.name = "";
    
    this->init = true;
    other.init = false;
  }
  
  Framebuffer &Framebuffer::operator = (Framebuffer&& other) noexcept
  {
    this->handle = other.handle;
    other.handle = INVALID_HANDLE;
    
    this->colorHandle = other.colorHandle;
    other.colorHandle = INVALID_HANDLE;
    
    this->depthHandle = other.depthHandle;
    other.depthHandle = INVALID_HANDLE;
    
    this->stencilHandle = other.stencilHandle;
    other.stencilHandle = INVALID_HANDLE;
    
    this->width = other.width;
    other.width = 0;
    
    this->height = other.height;
    other.height = 0;
    
    this->hasColorTex = other.hasColorTex;
    other.hasColorTex = false;
    
    this->hasDepthTex = other.hasDepthTex;
    other.hasDepthTex = false;
    
    this->hasAlpha = other.hasAlpha;
    other.hasAlpha = false;
    
    this->hasStencilTex = other.hasStencilTex;
    other.hasStencilTex = false;
    
    this->name = other.name;
    other.name = "";
    
    this->init = true;
    other.init = false;
    
    return *this;
  }
  
  bool Framebuffer::isValid() const
  {
    return this->init &&
    this->handle != INVALID_HANDLE &&
    (this->hasColorTex || this->hasDepthTex || this->hasStencilTex) &&
      ((this->hasColorTex && this->colorHandle != INVALID_HANDLE) ||
      (this->hasDepthTex && this->depthHandle != INVALID_HANDLE) ||
      (this->hasStencilTex && this->stencilHandle != INVALID_HANDLE));
  }
  
  bool Framebuffer::exists() const
  {
    return this->init;
  }
  
  void Framebuffer::reset()
  {
    this->handle = INVALID_HANDLE;
    this->colorHandle = INVALID_HANDLE;
    this->depthHandle = INVALID_HANDLE;
    this->stencilHandle = INVALID_HANDLE;
    this->width = INVALID_HANDLE;
    this->height = INVALID_HANDLE;
    this->hasColorTex = false;
    this->hasDepthTex = false;
    this->hasAlpha = false;
    this->hasStencilTex = false;
    this->name = "";
    this->init = false;
  }
  
  void Framebuffer::use() const
  {
    if(!this->init)
    {
      return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, this->handle);
  }
  
  void Framebuffer::bind(const GLAttachment type, const uint32_t target) const
  {
    switch(type)
    {
      case GLAttachment::COLOR_TEXTURE:
      {
        glBindTextureUnit(target, this->colorHandle);
        break;
      }
      case GLAttachment::DEPTH_TEXTURE:
      {
        glBindTextureUnit(target, this->depthHandle);
        break;
      }
      case GLAttachment::STENCIL_TEXTURE:
      {
        glBindTextureUnit(target, this->stencilHandle);
        break;
      }
      case GLAttachment::COLOR_RENDERBUFFER:
      {
        glBindRenderbuffer(target, this->colorHandle);
      }
      case GLAttachment::DEPTH_RENDERBUFFER:
      {
        glBindRenderbuffer(target, this->depthHandle);
      }
      case GLAttachment::STENCIL_RENDERBUFFER:
      {
        glBindRenderbuffer(target, this->stencilHandle);
      }
      default: break;
    }
  }
  
  void Framebuffer::regenerate(const uint32_t width, const uint32_t height)
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
    glViewport(0, 0, (GLsizei)this->width, (GLsizei)this->height);
    glScissor(0, 0, (GLsizei)this->width, (GLsizei)this->height);
    
    if(this->hasColorTex)
    {
      glCreateTextures(GL_TEXTURE_2D, 1, &this->colorHandle);
    }
    if(this->hasDepthTex)
    {
      glCreateTextures(GL_TEXTURE_2D, 1, &this->depthHandle);
    }
    
    glTextureStorage2D(this->colorHandle, 1, this->hasAlpha ? GL_RGBA32F : GL_RGB32F, (GLsizei)this->width, (GLsizei)this->height);
    glNamedFramebufferTexture(this->handle, GL_COLOR_ATTACHMENT0, this->colorHandle, 0);
    if(this->hasDepthTex)
    {
      glTextureStorage2D(this->depthHandle, 1, GL_DEPTH_COMPONENT32F, (GLsizei)this->width, (GLsizei)this->height);
      glNamedFramebufferTexture(this->handle, GL_DEPTH_ATTACHMENT, this->depthHandle, 0);
    }
    
    std::vector<GLenum> drawBuffers;
    drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
    if(this->hasDepthTex) drawBuffers.emplace_back(GL_COLOR_ATTACHMENT1);
    glNamedFramebufferDrawBuffers(this->handle, (int32_t)(drawBuffers.size()), drawBuffers.data());
    const GLenum error = glCheckNamedFramebufferStatus(this->handle, GL_FRAMEBUFFER);
    if(error != GL_FRAMEBUFFER_COMPLETE)
    {
      std::string er = "Framebuffer Creation Error: ";
      switch(error)
      {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        {
          er += "incomplete attachment";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        {
          er += "incomplete dimensions";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        {
          er += "missing attachment";
          break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED:
        {
          er += "Framebuffers are not supported";
          break;
        }
        default: break;
      }
      printf("%s\n", er.c_str());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void Framebuffer::clearFBO() const
  {
    glDeleteFramebuffers(1, &this->handle);
    glDeleteTextures(1, &this->colorHandle);
    glDeleteTextures(1, &this->depthHandle);
  }
  
  FramebufferPool::FramebufferPool(const size_t alloc, const uint32_t width, const uint32_t height)
  {
    this->pool.resize(alloc);
    for(size_t i = 0; i < alloc; i++)
    {
      this->pool[i] = Framebuffer(width, height, std::initializer_list{GLAttachment::COLOR_TEXTURE, GLAttachment::ALPHA_TEXTURE, GLAttachment::DEPTH_TEXTURE}, "Pool " + std::to_string(i));
    }
    this->init = true;
  }
  
  FramebufferPool::FramebufferPool(FramebufferPool&& moveFrom) noexcept
  {
    this->pool = std::move(moveFrom.pool);
    moveFrom.pool.clear();
    
    this->init = true;
    moveFrom.init = false;
  }
  
  FramebufferPool& FramebufferPool::operator=(FramebufferPool&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->pool = std::move(moveFrom.pool);
    moveFrom.pool.clear();
    
    this->init = true;
    moveFrom.init = false;
    
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
  
  Framebuffer& FramebufferPool::getNextAvailableFBO(const uint32_t width, const uint32_t height)
  {
    Framebuffer out;
    for(auto &fbo: this->pool)
    {
      if(fbo.exists())
      {
        if(fbo.width != width || fbo.height != height)
        {
          fbo = Framebuffer(width, height, std::initializer_list{GLAttachment::COLOR_TEXTURE, GLAttachment::ALPHA_TEXTURE, GLAttachment::DEPTH_TEXTURE}, "Pool " + std::to_string(this->pool.size() + 1));
        }
        fbo.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return fbo;
      }
    }
    this->pool.emplace_back(width, height, std::initializer_list{GLAttachment::COLOR_TEXTURE, GLAttachment::ALPHA_TEXTURE, GLAttachment::DEPTH_TEXTURE}, "Pool " + std::to_string(this->pool.size() + 1));
    this->pool.back().use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return this->pool.back();
  }
  
  void FramebufferPool::onResize(const uint32_t width, const uint32_t height)
  {
    for(auto &fbo: this->pool)
    {
      fbo.regenerate(width, height);
    }
  }
}
