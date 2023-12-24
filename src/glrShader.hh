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
    Shader() = default;
    
    GLRENDER_API Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader);
    GLRENDER_API Shader(std::string const &name, std::string const &compShader);
    GLRENDER_API ~Shader();
    
    Shader(Shader const &copyFrom) = delete;
    Shader& operator=(Shader const &copyFrom) = delete;
    GLRENDER_API Shader(Shader &&moveFrom) noexcept;
    GLRENDER_API Shader& operator=(Shader &&moveFrom) noexcept;
    
    GLRENDER_API void use() const;
    [[nodiscard]] GLRENDER_API int32_t getUniformHandle(std::string const &location);
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void sendFloat(std::string const &location, float val);
    GLRENDER_API void sendInt(std::string const &location, int32_t val);
    GLRENDER_API void sendUInt(std::string const &location, uint32_t val);
    GLRENDER_API void sendVec2f(std::string const &location, float *val);
    GLRENDER_API void sendVec3f(std::string const &location, float *val);
    GLRENDER_API void sendVec4f(std::string const &location, float *val);
    GLRENDER_API void sendMat3f(std::string const &location, float *val);
    GLRENDER_API void sendMat4f(std::string const &location, float *val);
    
    uint32_t handle = std::numeric_limits<uint32_t>::max();
    std::unordered_map<std::string, int32_t> uniforms = {};
    
    private:
    bool init = false;
  };
}
