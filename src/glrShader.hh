#pragma once

#include "export.hh"

#include <vector>
#include <string>
#include <unordered_map>

namespace glr
{
  /// An OpenGL frag/vert, geometry, or compute shader
  struct Shader
  {
    Shader() = delete;
    
    /*GLRENDER_API Shader(std::string const &name, std::vector<uint8_t> const &vertShader, std::vector<uint8_t> const &fragShader);
    GLRENDER_API Shader(std::string const &name, std::vector<uint8_t> const &compShader);*/
    GLRENDER_API Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader);
    GLRENDER_API Shader(std::string const &name, std::string const &compShader);
    GLRENDER_API ~Shader();
    
    GLRENDER_API Shader(Shader &other);
    GLRENDER_API Shader &operator =(Shader other);
    GLRENDER_API Shader(Shader &&other) noexcept;
    GLRENDER_API Shader &operator =(Shader &&other) noexcept;
    
    GLRENDER_API void use() const;
    [[nodiscard]] GLRENDER_API int32_t getUniformHandle(std::string const &location);
    GLRENDER_API void sendFloat(std::string const &location, float val);
    GLRENDER_API void sendInt(std::string const &location, int32_t val);
    GLRENDER_API void sendUInt(std::string const &location, uint32_t val);
    GLRENDER_API void sendVec2f(std::string const &location, float *val);
    GLRENDER_API void sendVec3f(std::string const &location, float *val);
    GLRENDER_API void sendVec4f(std::string const &location, float *val);
    GLRENDER_API void sendMat3f(std::string const &location, float *val);
    GLRENDER_API void sendMat4f(std::string const &location, float *val);
    
    uint32_t m_handle = 0;
    std::unordered_map<std::string, int32_t> m_uniforms = {};
  };
}
