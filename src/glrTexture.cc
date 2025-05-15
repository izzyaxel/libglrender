#include "glrender/glrTexture.hh"

#include <glad/gl.hh>

namespace glr
{
  //Used with subImage(), ie for atlases
  Texture::Texture(const std::string& name, const uint32_t width, const uint32_t height, const uint8_t channels, const GLRFilterMode min, const GLRFilterMode mag, const bool sRGB)
  {
    this->name = name;
    this->channels = channels;
    this->width = width;
    this->height = height;
    
    int32_t internalFormat = 0;
    switch(channels)
    {
      case 1:
      {
        internalFormat = GL_R8;
        break;
      }
      case 3: //TODO It's more efficient to use an extra 8 bits of VRAM per pixel
      {
        if(sRGB)
        {
          internalFormat = GL_SRGB8;
        }
        else
        {
          internalFormat = GL_RGB8;
        }
        break;
      }
      case 4:
      {
        if(sRGB)
        {
          internalFormat = GL_SRGB8_ALPHA8;
        }
        else
        {
          internalFormat = GL_RGBA8;
        }
        break;
      }
      default: break;
    }
    
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    glTextureStorage2D(this->handle, 1, internalFormat, (int32_t)width, (int32_t)height);
    
    this->clear();
    this->setFilterMode(min, mag);
    this->setAnisotropyLevel(1);
    this->init = true;
  }
  
  Texture::Texture(const std::string& name, const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t channels, const GLRFilterMode min, const GLRFilterMode mag, const bool sRGB)
  {
    this->name = name;
    this->channels = channels;
    this->width = width;
    this->height = height;
    
    int32_t internalFormat = 0;
    int32_t colorFormat = 0;
    switch(channels)
    {
      case 1:
      {
        internalFormat = GL_R8;
        colorFormat = GL_RED;
        break;
      }
      case 3: //TODO It's more efficient to use an extra 8 bits of VRAM per pixel
      {
        if(sRGB)
        {
          internalFormat = GL_SRGB8;
        }
        else
        {
          internalFormat = GL_RGB8;
        }
        colorFormat = GL_RGB;
        break;
      }
      case 4:
      {
        if(sRGB)
        {
          internalFormat = GL_SRGB8_ALPHA8;
        }
        else
        {
          internalFormat = GL_RGBA8;
        }
        colorFormat = GL_RGBA;
        break;
      }
      default: break;
    }
    
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    glTextureStorage2D(this->handle, 1, internalFormat, (int32_t)width, (int32_t)height);
    glTextureSubImage2D(this->handle, 0, 0, 0, (int32_t)this->width, (int32_t)this->height, colorFormat, GL_UNSIGNED_BYTE, data);
    
    this->setFilterMode(min, mag);
    this->setAnisotropyLevel(1);
    this->init = true;
  }
  
  Texture::Texture(const std::string& name, const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha, const bool sRGB)
  {
    this->name = name;
    this->channels = 4;
    this->width = 1;
    this->height = 1;
    
    uint8_t **data = new uint8_t *;
    data[0] = new uint8_t[4];
    data[0][0] = red;
    data[0][1] = green;
    data[0][2] = blue;
    data[0][3] = alpha;
    
    glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
    glTextureStorage2D(this->handle, 1, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, (int32_t)this->width, (int32_t)this->height);
    glTextureSubImage2D(this->handle, 0, 0, 0, (int32_t)this->width, 1, GL_RGBA, GL_UNSIGNED_BYTE, data[0]);
    
    this->setFilterMode(GLRFilterMode::BILINEAR, GLRFilterMode::BILINEAR);
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
    moveFrom.handle = INVALID_HANDLE;
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->channels = moveFrom.channels;
    moveFrom.channels = {};

    this->bindingIndex = moveFrom.bindingIndex;
    moveFrom.bindingIndex = 0;
    
    this->name = moveFrom.name;
    moveFrom.name = "";
    
    this->path = moveFrom.path;
    moveFrom.path = "";
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Texture& Texture::operator=(Texture&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->handle = moveFrom.handle;
    moveFrom.handle = INVALID_HANDLE;
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->channels = moveFrom.channels;
    moveFrom.channels = {};

    this->bindingIndex = moveFrom.bindingIndex;
    moveFrom.bindingIndex = 0;
    
    this->name = moveFrom.name;
    moveFrom.name = "";
    
    this->path = moveFrom.path;
    moveFrom.path = "";
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  bool Texture::isValid() const
  {
    return this->exists() && this->handle != INVALID_HANDLE;
  }
  
  bool Texture::exists() const
  {
    return this->init;
  }
  
  void Texture::reset()
  {
    glDeleteTextures(1, &this->handle);
    this->handle = INVALID_HANDLE;
    this->width = 0;
    this->height = 0;
    this->channels = {};
    this->name = "";
    this->path = "";
    this->init = false;
  }
  
  void Texture::use() const
  {
    int32_t curTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);
    if(curTex != (int32_t)this->bindingIndex)
    {
      switch(this->bindingType)
      {
        case GLRShaderType::FRAG_VERT:
        {
          glBindTextureUnit(this->bindingIndex, this->handle);
          break;
        }
        case GLRShaderType::COMPUTE:
        {
          glBindImageTexture(this->bindingIndex, this->handle, 0, GL_FALSE, 0, (uint32_t)this->bindingIOMode, (uint32_t)this->bindingColorFormat);
          break;
        }
        default: break;
      }
      
    }
  }
  
  void Texture::setFilterMode(const GLRFilterMode min, const GLRFilterMode mag) const
  {
    GLenum glMin = GL_NEAREST, glMag = GL_NEAREST;
    switch(min)
    {
      case GLRFilterMode::NEAREST:
      {
        glMin = GL_NEAREST;
        break;
      }
      case GLRFilterMode::BILINEAR:
      {
        glMin = GL_LINEAR;
        break;
      }
      case GLRFilterMode::TRILINEAR:
      {
        glMin = GL_LINEAR_MIPMAP_LINEAR;
        break;
      }
      default: break;
    }
    
    switch(mag)
    {
      case GLRFilterMode::NEAREST:
      {
        glMag = GL_NEAREST;
        break;
      }
      case GLRFilterMode::BILINEAR:
      {
        glMag = GL_LINEAR;
        break;
      }
      case GLRFilterMode::TRILINEAR:
      {
        glMag = GL_LINEAR_MIPMAP_LINEAR;
        break;
      }
      default: break;
    }
    glTextureParameteri(this->handle, GL_TEXTURE_MIN_FILTER, (GLint)glMin);
    glTextureParameteri(this->handle, GL_TEXTURE_MAG_FILTER, (GLint)glMag);
  }
  
  void Texture::setAnisotropyLevel(const uint32_t level) const
  {
    glTextureParameterf(this->handle, GL_TEXTURE_MAX_ANISOTROPY, (GLfloat)level);
  }
  
  void Texture::subImage(const uint8_t* data, const uint32_t w, const uint32_t h, const uint32_t xPos, const uint32_t yPos, const uint8_t channels) const
  {
    int32_t format = 0;
    switch(channels)
    {
      case 3:
      {
        format = GL_RGB;
        break;
      }
      case 4:
      {
        format = GL_RGBA;
        break;
      }
      case 1:
      {
        format = GL_RED;
        break;
      }
      default: break;
    }
    glTextureSubImage2D(this->handle, 0, (GLint)xPos, (GLint)yPos, (GLint)w, (GLint)h, format, GL_UNSIGNED_BYTE, data);
  }
  
  void Texture::clear() const
  {
    int32_t format = 0;
    switch(this->channels)
    {
      case 3:
      {
        format = GL_RGB;
        break;
      }
      case 4:
      {
        format = GL_RGBA;
        break;
      }
      case 1:
      {
        format = GL_RED;
        break;
      }
      default: break;
    }
    glClearTexImage(this->handle, 0, format, GL_UNSIGNED_BYTE, "\0\0\0\0");
  }
  
  DownloadedImageData Texture::downloadTexture(const uint8_t channels) const
  {
    DownloadedImageData out;
    out.textureName = this->name;
    int32_t currentTexture = 0;
    int32_t format = 0;
    int32_t channelsPerPixel = 0;
    switch(channels)
    {
      case 3:
      {
        format = GL_RGB;
        channelsPerPixel = 3;
        break;
      }
      case 4:
      {
        format = GL_RGBA;
        channelsPerPixel = 4;
        break;
      }
      case 1:
      {
        format = GL_RED;
        channelsPerPixel = 1;
        break;
      }
      default: break;
    }
    
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
    glBindTextureUnit(0, this->handle);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &out.width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &out.height);
    out.imageData.resize(out.width * out.height * channelsPerPixel);
    glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)format, GL_UNSIGNED_BYTE, out.imageData.data());
    glBindTextureUnit(0, currentTexture);
    return out;
  }
}
