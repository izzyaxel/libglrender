#include "glrender/glrFramebuffer.hh"

#include <algorithm>
#include <glad/gl.hh>

namespace glr
{
  Framebuffer::~Framebuffer()
  {
    this->clear();
  }

  //TODO FIXME transfer other variables
  Framebuffer::Framebuffer(Framebuffer&& other) noexcept
  {
    this->framebufferHandle = other.framebufferHandle;
    other.framebufferHandle = INVALID_HANDLE;
    
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
    
    this->hasColor = other.hasColor;
    other.hasColor = false;
    
    this->hasDepth = other.hasDepth;
    other.hasDepth = false;
    
    this->hasStencil = other.hasStencil;
    other.hasStencil = false;
  }
  
  Framebuffer &Framebuffer::operator = (Framebuffer&& other) noexcept
  {
    this->framebufferHandle = other.framebufferHandle;
    other.framebufferHandle = INVALID_HANDLE;
    
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
    
    this->hasColor = other.hasColor;
    other.hasColor = false;
    
    this->hasDepth = other.hasDepth;
    other.hasDepth = false;
    
    this->hasStencil = other.hasStencil;
    other.hasStencil = false;
    
    return *this;
  }

  Framebuffer* Framebuffer::setDimensions(const uint32_t width, const uint32_t height)
  {
    this->width = width;
    this->height = height;
    return this;
  }

  Framebuffer* Framebuffer::addColorAttachment(const GLRAttachmentType attachmentType, const uint8_t channels)
  {
    this->hasColor = true;
    this->colorChannels = channels;
    switch(attachmentType)
    {
      case GLRAttachmentType::TEXTURE:
      {
        this->colorType = GLRAttachmentType::TEXTURE;
        break;
      }
      case GLRAttachmentType::RENDER_BUFFER:
      {
        this->colorType = GLRAttachmentType::RENDER_BUFFER;
        break;
      }
      default: break;
    }
    return this;
  }

  Framebuffer* Framebuffer::addDepthAttachment(const GLRAttachmentType attachmentType)
  {
    this->hasDepth = true;
    switch(attachmentType)
    {
      case GLRAttachmentType::TEXTURE:
      {
        this->depthType = GLRAttachmentType::TEXTURE;
        break;
      }
      case GLRAttachmentType::RENDER_BUFFER:
      {
        this->depthType = GLRAttachmentType::RENDER_BUFFER;
        break;
      }
      default: break;
    }
    return this;
  }

  Framebuffer* Framebuffer::addStencilAttachment(const GLRAttachmentType attachmentType)
  {
    this->hasStencil = true;
    switch(attachmentType)
    {
      case GLRAttachmentType::TEXTURE:
      {
        this->stencilType = GLRAttachmentType::TEXTURE;
        break;
      }
      case GLRAttachmentType::RENDER_BUFFER:
      {
        this->stencilType = GLRAttachmentType::RENDER_BUFFER;
        break;
      }
      default: break;
    }
    return this;
  }
  
  void Framebuffer::finalize()
  {
    glCreateFramebuffers(1, &this->framebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferHandle);
    glViewport(0, 0, (GLsizei)this->width, (GLsizei)this->height);
    glScissor(0, 0, (GLsizei)this->width, (GLsizei)this->height);

    std::vector<GLenum> drawBuffers;
    
    //Attachments
    if(this->hasColor) //TODO learn how to handle renderbuffers
    {
      drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
      
      if(this->colorType == GLRAttachmentType::TEXTURE)
      {
        glCreateTextures(GL_TEXTURE_2D, 1, &this->colorHandle);
        switch(this->colorChannels)
        {
          case 1:
          {
            glTextureStorage2D(this->colorHandle, 1, GL_RED, (GLsizei)this->width, (GLsizei)this->height);
            glNamedFramebufferTexture(this->framebufferHandle, GL_COLOR_ATTACHMENT0, this->colorHandle, 0);
            break;
          }
          case 3:
          {
            glTextureStorage2D(this->colorHandle, 1, GL_RGB32F, (GLsizei)this->width, (GLsizei)this->height);
            glNamedFramebufferTexture(this->framebufferHandle, GL_COLOR_ATTACHMENT0, this->colorHandle, 0);
            break;
          }
          case 4:
          {
            glTextureStorage2D(this->colorHandle, 1, GL_RGBA32F, (GLsizei)this->width, (GLsizei)this->height);
            glNamedFramebufferTexture(this->framebufferHandle, GL_COLOR_ATTACHMENT0, this->colorHandle, 0);
            break;
          }
          default: break;
        }
      }
      else //Renderbuffer attachment
      {
        glGenRenderbuffers(1, &this->colorHandle);
      }
    }
    if(this->hasDepth)
    {
      drawBuffers.emplace_back(GL_DEPTH_ATTACHMENT);
      
      if(this->depthType == GLRAttachmentType::TEXTURE)
      {
        glCreateTextures(GL_TEXTURE_2D, 1, &this->colorHandle);
        glTextureStorage2D(this->depthHandle, 1, GL_DEPTH_COMPONENT32F, (GLsizei)this->width, (GLsizei)this->height);
        glNamedFramebufferTexture(this->framebufferHandle, GL_DEPTH_ATTACHMENT, this->depthHandle, 0);
      }
      else //Renderbuffer attachment
      {
        glGenRenderbuffers(1, &this->depthHandle);
      }
    }
    if(this->hasStencil)
    {
      drawBuffers.emplace_back(GL_STENCIL_ATTACHMENT);
      
      if(this->stencilType == GLRAttachmentType::TEXTURE)
      {
        glCreateTextures(GL_TEXTURE_2D, 1, &this->colorHandle);
        glTextureStorage2D(this->stencilHandle, 1, GL_STENCIL, (GLsizei)this->width, (GLsizei)this->height);
        glNamedFramebufferTexture(this->framebufferHandle, GL_STENCIL_ATTACHMENT, this->stencilHandle, 0);
      }
      else //Renderbuffer attachment
      {
        glGenRenderbuffers(1, &this->stencilHandle);
      }
    }

    glNamedFramebufferDrawBuffers(this->framebufferHandle, (int32_t)drawBuffers.size(), drawBuffers.data());
    
    const GLenum error = glCheckNamedFramebufferStatus(this->framebufferHandle, GL_FRAMEBUFFER);
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
    this->finalized = true;
  }
  
  void Framebuffer::use() const
  {
    if(!this->finalized)
    {
      return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferHandle);
  }
  
  void Framebuffer::bindAttachment(const GLRAttachment attachment, const GLRAttachmentType type, const uint32_t target) const
  {
    switch(attachment)
    {
      case GLRAttachment::COLOR:
      {
        if(this->hasColor && this->colorType == GLRAttachmentType::TEXTURE)
        {
          switch(type)
          {
            case GLRAttachmentType::TEXTURE:
            {
              glBindTextureUnit(target, this->colorHandle);
              break;
            }
            case GLRAttachmentType::RENDER_BUFFER:
            {
              glBindRenderbuffer(target, this->colorHandle);
              break;
            }
            default: break;
          }
        }
        break;
      }
      case GLRAttachment::DEPTH:
      {
        if(this->hasDepth && this->depthType == GLRAttachmentType::TEXTURE)
        {
          switch(type)
          {
            case GLRAttachmentType::TEXTURE:
            {
              glBindTextureUnit(target, this->depthHandle);
              break;
            }
            case GLRAttachmentType::RENDER_BUFFER:
            {
              glBindRenderbuffer(target, this->depthHandle);
              break;
            }
            default: break;
          }
        }
        break;
      }
      case GLRAttachment::STENCIL:
      {
        if(this->hasStencil && this->stencilType == GLRAttachmentType::TEXTURE)
        {
          switch(type)
          {
            case GLRAttachmentType::TEXTURE:
            {
              glBindTextureUnit(target, this->stencilHandle);
              break;
            }
            case GLRAttachmentType::RENDER_BUFFER:
            {
              glBindRenderbuffer(target, this->stencilHandle);
              break;
            }
            default: break;
          }
        }
        break;
      }
      default: break;
    }
  }
  
  void Framebuffer::resize(const uint32_t width, const uint32_t height)
  {
    const uint8_t colorChannels = this->colorChannels;
    const bool hasColor = this->hasColor;
    const bool hasDepth = this->hasDepth;
    const bool hasStencil = this->hasStencil;
    const GLRAttachmentType colorType = this->colorType;
    const GLRAttachmentType depthType = this->depthType;
    const GLRAttachmentType stencilType = this->stencilType;
    
    this->clear();
    
    this->setDimensions(width, height);
    if(hasColor)
    {
      this->addColorAttachment(colorType, colorChannels);
    }
    if(hasDepth)
    {
      this->addDepthAttachment(depthType);
    }
    if(hasStencil)
    {
      this->addStencilAttachment(stencilType);
    }
    this->finalize();
  }
  
  void Framebuffer::clear()
  {
    glDeleteFramebuffers(1, &this->framebufferHandle);

    if(this->hasColor)
    {
      if(this->colorType == GLRAttachmentType::TEXTURE)
      {
        glDeleteTextures(1, &this->colorHandle);
      }
      else
      {
        glDeleteRenderbuffers(1, &this->colorHandle);
      }
    }
    if(this->hasDepth)
    {
      if(this->depthType == GLRAttachmentType::TEXTURE)
      {
        glDeleteTextures(1, &this->depthHandle);
      }
      else
      {
        glDeleteRenderbuffers(1, &this->depthHandle);
      }
    }
    if(this->hasStencil)
    {
      if(this->stencilType == GLRAttachmentType::TEXTURE)
      {
        glDeleteTextures(1, &this->stencilHandle);
      }
      else
      {
        glDeleteRenderbuffers(1, &this->stencilHandle);
      }
    }

    this->framebufferHandle = INVALID_HANDLE;
    this->colorHandle = INVALID_HANDLE;
    this->depthHandle = INVALID_HANDLE;
    this->stencilHandle = INVALID_HANDLE;
    
    this->hasColor = false;
    this->hasDepth = false;
    this->hasStencil = false;
    this->finalized = false;
    
    this->colorType = GLRAttachmentType::TEXTURE;
    this->depthType = GLRAttachmentType::TEXTURE;
    this->stencilType = GLRAttachmentType::TEXTURE;

    this->colorChannels = 4;
    this->width = 0;
    this->height = 0;
  }


  FramebufferPool::FramebufferPool(const size_t alloc, const uint32_t width, const uint32_t height)
  {
    this->pool.resize(alloc);
    for(size_t i = 0; i < alloc; i++)
    {
      this->pool[i] = Framebuffer();
      auto& fbo = this->pool[i];
      fbo.setDimensions(width, height)->addColorAttachment(GLRAttachmentType::TEXTURE, 4)->addDepthAttachment(GLRAttachmentType::TEXTURE)->finalize();
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
      if(fbo.framebufferHandle != INVALID_HANDLE)
      {
        if(fbo.width != width || fbo.height != height)
        {
          fbo.setDimensions(width, height)->addColorAttachment(GLRAttachmentType::TEXTURE, 4)->addDepthAttachment(GLRAttachmentType::TEXTURE)->finalize();
        }
        fbo.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return fbo;
      }
    }
    this->pool.emplace_back();
    this->pool.back().setDimensions(width, height)->addColorAttachment(GLRAttachmentType::TEXTURE, 4)->addDepthAttachment(GLRAttachmentType::TEXTURE);
    this->pool.back().use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return this->pool.back();
  }
  
  void FramebufferPool::onResize(const uint32_t width, const uint32_t height)
  {
    for(auto &fbo: this->pool)
    {
      fbo.resize(width, height);
    }
  }
}
