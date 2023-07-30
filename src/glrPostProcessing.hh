#pragma once

#include "glrFramebuffer.hh"

#include <vector>
#include <memory>
#include <functional>

namespace glr
{
  typedef std::function<void(std::shared_ptr<Framebuffer>, std::shared_ptr<Framebuffer>, void * const)> ProcessFunc;
  
  struct PostPass
  {
    PostPass() = default;
    ProcessFunc m_process = nullptr;
    bool m_enabled = true;
    std::string m_name;
    void *m_userData = nullptr;
  };
  
  struct PostStack
  {
    GLRENDER_API void add(PostPass pass);
    [[nodiscard]] GLRENDER_API std::vector<PostPass> getPasses();
    [[nodiscard]] GLRENDER_API bool isEmpty() const;
    GLRENDER_API void clear();
    
    private:
    std::vector<PostPass> p_postOrder;
  };
}
