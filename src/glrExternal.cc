#include "GLRender/glrExternal.hh"

#include <glad/gl.hh>

namespace glr
{
  void pixelStoreiPack(const int i)
  {
    glPixelStorei(GL_PACK_ALIGNMENT, i);
  }
  
  void pixelStoreiUnpack(const int i)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, i);
  }
  
  std::vector<uint8_t> getPixels(const uint32_t width, const uint32_t height)
  {
    std::vector<uint8_t> out;
    out.resize(width * height * 3);
    glReadPixels(0, 0, (GLsizei)width, (GLsizei)height, GL_RGB, GL_UNSIGNED_BYTE, out.data());
    return out;
  }
}
