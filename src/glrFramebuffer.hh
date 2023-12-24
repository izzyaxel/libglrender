#pragma once

#include "export.hh"
#include "glrEnums.hh"

#include <string>
#include <vector>
#include <memory>

namespace glr
{
  /// An OpenGL framebuffer
  struct Framebuffer
  {
    Framebuffer() = default;
    GLRENDER_API Framebuffer(uint32_t width, uint32_t height, std::initializer_list<Attachment> const &options, std::string const &name);
    GLRENDER_API ~Framebuffer();
    
    Framebuffer(Framebuffer &other) = delete;
    Framebuffer& operator =(Framebuffer const &copyFrom) = delete;
    GLRENDER_API Framebuffer(Framebuffer &&moveFrom) noexcept;
    GLRENDER_API Framebuffer &operator =(Framebuffer &&moveFrom) noexcept;
    
    
    GLRENDER_API void use() const;
    GLRENDER_API void bind(Attachment type, uint32_t target) const;
    GLRENDER_API void regenerate(uint32_t width, uint32_t height);
    
    uint32_t handle = 0;
    uint32_t colorHandle = 0;
    uint32_t depthHandle = 0;
    uint32_t stencilHandle = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    bool hasColor = false;
    bool hasDepth = false;
    bool hasAlpha = false;
    bool hasStencil = false;
    std::string name;
    
    private:
    void createFBO();
    void clearFBO();
  };
  
  struct FramebufferPool
  {
    FramebufferPool() = delete;
    FramebufferPool(FramebufferPool const &other) = delete;
    FramebufferPool(FramebufferPool const &&other) = delete;
    GLRENDER_API FramebufferPool(size_t alloc, uint32_t width, uint32_t height);
    
    [[nodiscard]] GLRENDER_API std::shared_ptr<Framebuffer> getNextAvailableFBO(uint32_t width, uint32_t height);
    GLRENDER_API void onResize(uint32_t width, uint32_t height);
    
    private:
    std::vector<std::shared_ptr<Framebuffer>> pool;
  };
}
