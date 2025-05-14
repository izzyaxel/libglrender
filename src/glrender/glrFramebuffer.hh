#pragma once

#include "export.hh"
#include "glrEnums.hh"
#include "glrUtil.hh"

#include <vector>
#include <memory>

namespace glr
{
  //TODO implement RenderBuffer attachments for fast FBO transfers/double buffering
  /// An OpenGL framebuffer
  struct Framebuffer
  {
    Framebuffer() = default;
    GLRENDER_API ~Framebuffer();
    
    Framebuffer(Framebuffer& other) = delete;
    Framebuffer& operator =(const Framebuffer& other) = delete;
    GLRENDER_API Framebuffer(Framebuffer&& other) noexcept;
    GLRENDER_API Framebuffer &operator =(Framebuffer&& other) noexcept;

    GLRENDER_API Framebuffer* setDimensions(uint32_t width, uint32_t height);
    GLRENDER_API Framebuffer* addColorAttachment(GLRAttachmentType attachmentType, uint8_t channels);
    GLRENDER_API Framebuffer* addDepthAttachment(GLRAttachmentType attachmentType);
    GLRENDER_API Framebuffer* addStencilAttachment(GLRAttachmentType attachmentType);
    GLRENDER_API void finalize();

    GLRENDER_API void use() const;
    GLRENDER_API void bindAttachment(GLRAttachment attachment, GLRAttachmentType type, uint32_t target) const;
    GLRENDER_API void resize(uint32_t width, uint32_t height);
    GLRENDER_API void clear();

    uint32_t width = 0;
    uint32_t height = 0;
    
    uint32_t framebufferHandle = INVALID_HANDLE;
    uint32_t colorHandle = INVALID_HANDLE;
    uint32_t depthHandle = INVALID_HANDLE;
    uint32_t stencilHandle = INVALID_HANDLE;

    uint8_t colorChannels = 4;
    
    bool hasColor = false;
    bool hasDepth = false;
    bool hasStencil = false;
    bool finalized = false;
    
    GLRAttachmentType colorType = GLRAttachmentType::TEXTURE;
    GLRAttachmentType depthType = GLRAttachmentType::TEXTURE;
    GLRAttachmentType stencilType = GLRAttachmentType::TEXTURE;
  };
  
  struct FramebufferPool
  {
    GLRENDER_API FramebufferPool() = default;
    GLRENDER_API FramebufferPool(size_t alloc, uint32_t width, uint32_t height);
    
    FramebufferPool(const FramebufferPool& other) = delete;
    FramebufferPool& operator=(const FramebufferPool& other) = delete;
    GLRENDER_API FramebufferPool(FramebufferPool&& moveFrom) noexcept;
    GLRENDER_API FramebufferPool& operator=(FramebufferPool&& moveFrom) noexcept;
    
    GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API Framebuffer& getNextAvailableFBO(uint32_t width, uint32_t height);
    GLRENDER_API void onResize(uint32_t width, uint32_t height);
    
    private:
    std::vector<Framebuffer> pool;
    bool init = false;
  };
}
