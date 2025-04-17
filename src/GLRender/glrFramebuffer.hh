#pragma once

#include "export.hh"
#include "glrUtil.hh"

#include <string>
#include <vector>
#include <memory>

namespace glr
{
  typedef enum
  {
    COLOR, ALPHA, DEPTH, STENCIL,
  } Attachment;
  
  /// An OpenGL framebuffer
  struct Framebuffer
  {
    Framebuffer() = default;
    GLRENDER_API Framebuffer(uint32_t width, uint32_t height, std::initializer_list<Attachment> options, const std::string& name);
    GLRENDER_API ~Framebuffer();
    
    Framebuffer(Framebuffer& other) = delete;
    Framebuffer& operator =(const Framebuffer& other) = delete;
    GLRENDER_API Framebuffer(Framebuffer&& other) noexcept;
    GLRENDER_API Framebuffer &operator =(Framebuffer&& other) noexcept;
    
    [[nodiscard]] GLRENDER_API bool isValid() const;
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use() const;
    GLRENDER_API void bind(Attachment type, uint32_t target) const;
    GLRENDER_API void regenerate(uint32_t width, uint32_t height);
    
    uint32_t handle = INVALID_HANDLE;
    uint32_t colorHandle = INVALID_HANDLE;
    uint32_t depthHandle = INVALID_HANDLE;
    uint32_t stencilHandle = INVALID_HANDLE;
    uint32_t width = 0;
    uint32_t height = 0;
    bool hasColor = false;
    bool hasDepth = false;
    bool hasAlpha = false;
    bool hasStencil = false;
    std::string name;
    
    private:
    void createFBO();
    void clearFBO() const;
    bool init = false;
  };
  
  struct FramebufferPool
  {
    GLRENDER_API FramebufferPool() = default;
    GLRENDER_API FramebufferPool(size_t alloc, uint32_t width, uint32_t height);
    
    FramebufferPool(const FramebufferPool& other) = delete;
    FramebufferPool& operator=(const FramebufferPool& other) = delete;
    GLRENDER_API FramebufferPool(FramebufferPool&& moveFrom) noexcept;
    GLRENDER_API FramebufferPool& operator=(FramebufferPool&& moveFrom) noexcept;
    
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    [[nodiscard]] GLRENDER_API Framebuffer& getNextAvailableFBO(uint32_t width, uint32_t height);
    GLRENDER_API void onResize(uint32_t width, uint32_t height);
    
    private:
    std::vector<Framebuffer> pool;
    bool init = false;
  };
}
