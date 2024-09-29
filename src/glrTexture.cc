#include "glrTexture.hh"

#include <glad/gl.hh>

namespace glr
{
  Texture::Texture(const std::string& name, const uint32_t width, const uint32_t height, const TexColorFormat colorFormat, const FilterMode mode, const bool sRGB)
  {
    this->name = name;
    this->fmt = colorFormat;
    this->width = width;
    this->height = height;
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    int32_t f = 0;
    if(colorFormat == TexColorFormat::RGB) //It's more efficient to use an extra 8 bits of VRAM per pixel
    {
      if(sRGB) f = GL_SRGB8;
      else f = GL_RGB8;
    }
    else if(colorFormat == TexColorFormat::RGBA)
    {
      if(sRGB) f = GL_SRGB8_ALPHA8;
      else f = GL_RGBA8;
    }
    else if(colorFormat == TexColorFormat::GREY)
    {
      f = GL_R8;
    }
    glTextureStorage2D(this->handle, 1, f, (GLsizei) width, (GLsizei) height);
    this->clear();
    this->setFilterMode(mode, mode);
    this->setAnisotropyLevel(1);
    
    this->init = true;
  }
  
  Texture::Texture(const std::string& name, const uint8_t* data, const uint32_t width, const uint32_t height, const TexColorFormat colorFormat, const FilterMode mode, const bool sRGB)
  {
    this->name = name;
    this->fmt = colorFormat;
    this->width = width;
    this->height = height;
    int32_t f = 0, cf = 0;
    if(colorFormat == TexColorFormat::RGB) //TODO It's more efficient to use an extra 8 bits of VRAM per pixel
    {
      if(sRGB) f = GL_SRGB8;
      else f = GL_RGB8;
      cf = GL_RGB;
    }
    else if(colorFormat == TexColorFormat::RGBA)
    {
      if(sRGB) f = GL_SRGB8_ALPHA8;
      else f = GL_RGBA8;
      cf = GL_RGBA;
    }
    else if(colorFormat == TexColorFormat::GREY)
    {
      f = GL_R8;
      cf = GL_RED;
    }
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    glTextureStorage2D(this->handle, 1, f, (GLsizei) width, (GLsizei) height);
    glTextureSubImage2D(this->handle, 0, 0, 0, (GLsizei) this->width, (GLsizei) this->height, cf, GL_UNSIGNED_BYTE, data);
    this->setFilterMode(mode, mode);
    this->setAnisotropyLevel(1);
    this->init = true;
  }
  
  Texture::Texture(const std::string& name, const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha, const bool sRGB)
  {
    this->name = name;
    this->fmt = TexColorFormat::RGBA;
    this->width = 1;
    this->height = 1;
    uint8_t **data = new uint8_t *;
    data[0] = new uint8_t[4];
    data[0][0] = red;
    data[0][1] = green;
    data[0][2] = blue;
    data[0][3] = alpha;
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    glTextureStorage2D(this->handle, 1, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, (GLsizei) this->width, (GLsizei) this->height);
    glTextureSubImage2D(this->handle, 0, 0, 0, (GLsizei) this->width, 1, GL_RGBA, GL_UNSIGNED_BYTE, data[0]);
    this->setFilterMode(FilterMode::BILINEAR, FilterMode::BILINEAR);
    this->setAnisotropyLevel(1);
    this->init = true;
  }
  
  Texture::~Texture()
  {
    glDeleteTextures(1, &this->handle);
  }
  
  Texture::Texture(Texture&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->fmt = moveFrom.fmt;
    moveFrom.fmt = {};
    
    this->name = moveFrom.name;
    moveFrom.name = "";
    
    this->path = moveFrom.path;
    moveFrom.path = "";
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Texture& Texture::operator=(Texture&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->fmt = moveFrom.fmt;
    moveFrom.fmt = {};
    
    this->name = moveFrom.name;
    moveFrom.name = "";
    
    this->path = moveFrom.path;
    moveFrom.path = "";
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  bool Texture::exists() const
  {
    return this->init;
  }
  
  void Texture::reset()
  {
    glDeleteTextures(1, &this->handle);
    this->handle = std::numeric_limits<uint32_t>::max();
    this->width = 0;
    this->height = 0;
    this->fmt = {};
    this->name = "";
    this->path = "";
    this->init = false;
  }
  
  void Texture::use(const uint32_t target) const
  {
    int32_t curTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);
    if(curTex != (int32_t) target)
    {
      glBindTextureUnit(target, this->handle);
    }
  }
  
  void Texture::setFilterMode(const FilterMode min, const FilterMode mag) const
  {
    GLenum glMin = GL_NEAREST, glMag = GL_NEAREST;
    switch(min)
    {
      case FilterMode::NEAREST:
        glMin = GL_NEAREST;
        break;
      case FilterMode::BILINEAR:
        glMin = GL_LINEAR;
        break;
      case FilterMode::TRILINEAR:
        glMin = GL_LINEAR_MIPMAP_LINEAR;
        break;
      default:
        break;
    }
    
    switch(mag)
    {
      case FilterMode::NEAREST:
        glMag = GL_NEAREST;
        break;
      case FilterMode::BILINEAR:
        glMag = GL_LINEAR;
        break;
      case FilterMode::TRILINEAR:
        glMag = GL_LINEAR_MIPMAP_LINEAR;
        break;
      default:
        break;
    }
    glTextureParameteri(this->handle, GL_TEXTURE_MIN_FILTER, (GLint) glMin);
    glTextureParameteri(this->handle, GL_TEXTURE_MAG_FILTER, (GLint) glMag);
  }
  
  void Texture::setAnisotropyLevel(const uint32_t level) const
  {
    glTextureParameterf(this->handle, GL_TEXTURE_MAX_ANISOTROPY, (GLfloat) level);
  }
  
  void Texture::subImage(const uint8_t* data, const uint32_t w, const uint32_t h, const uint32_t xPos, const uint32_t yPos, const TexColorFormat format) const
  {
    int32_t f = 0;
    switch(format)
    {
      case TexColorFormat::RGB:
        f = GL_RGB;
        break;
      case TexColorFormat::RGBA:
        f = GL_RGBA;
        break;
      case TexColorFormat::GREY:
        f = GL_RED;
        break;
    }
    glTextureSubImage2D(this->handle, 0, (GLint) xPos, (GLint) yPos, (GLint) w, (GLint) h, f, GL_UNSIGNED_BYTE, data);
  }
  
  void Texture::clear() const
  {
    int32_t f = 0;
    switch(this->fmt)
    {
      case TexColorFormat::RGB:
        f = GL_RGB;
        break;
      case TexColorFormat::RGBA:
        f = GL_RGBA;
        break;
      case TexColorFormat::GREY:
        f = GL_RED;
        break;
    }
    glClearTexImage(this->handle, 0, f, GL_UNSIGNED_BYTE, "\0\0\0\0");
  }
  
  DownloadedImageData Texture::downloadTexture(const TexColorFormat colorFormat) const
  {
    DownloadedImageData out;
    out.textureName = this->name;
    int32_t curTex = 0;
    int32_t cf = 0;
    int32_t cpp = 0;
    switch(colorFormat)
    {
      case TexColorFormat::RGB:
        cf = GL_RGB;
        cpp = 3;
        break;
      case TexColorFormat::RGBA:
        cf = GL_RGBA;
        cpp = 4;
        break;
      case TexColorFormat::GREY:
        cf = GL_RED;
        cpp = 1;
        break;
      default:
        break;
    }
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);
    glBindTextureUnit(0, this->handle);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &out.width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &out.height);
    out.imageData.resize(out.width * out.height * cpp);
    glGetTexImage(GL_TEXTURE_2D, 0, (GLenum) cf, GL_UNSIGNED_BYTE, out.imageData.data());
    glBindTextureUnit(0, curTex);
    return out;
  }
}
