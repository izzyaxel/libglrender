#pragma once

#include "glrFramebuffer.hh"

#include <vector>
#include <memory>
#include <functional>

namespace glr
{
  typedef std::function<void(Framebuffer&, Framebuffer&, const void*)> ProcessFunc;
  
  struct PostPass
  {
    PostPass() = default;
    ProcessFunc process = nullptr;
    bool enabled = true;
    std::string name;
    void *userData = nullptr;
  };
  
  struct PostStack
  {
    GLRENDER_API void add(PostPass pass);
    GLRENDER_API std::vector<PostPass> getPasses();
    GLRENDER_API bool isEmpty() const;
    GLRENDER_API void clear();
    
    private:
    std::vector<PostPass> postOrder{};
  };
}
