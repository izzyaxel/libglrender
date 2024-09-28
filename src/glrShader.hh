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
    struct Uniform
    {
      int32_t handle = std::numeric_limits<int32_t>::max();
      enum Type
      {
        INVALID = 0, FLOAT, INT, UINT, VEC2, VEC3, VEC4, MAT3, MAT4,
      } type = INVALID;
      float valF = 0;
      int32_t valI = 0;
      uint32_t valUI = 0;
      vec2<float> valVec2 = {};
      vec3<float> valVec3 = {};
      vec4<float> valVec4 = {};
      mat3x3<float> valMat3 = {};
      mat4x4<float> valMat4 = {};
    };
    
    Shader() = default;
    
    GLRENDER_API Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader);
    GLRENDER_API Shader(std::string const &name, std::string const &compShader);
    GLRENDER_API ~Shader();
    
    Shader(Shader const &copyFrom) = delete;
    Shader& operator=(Shader const &copyFrom) = delete;
    GLRENDER_API Shader(Shader &&moveFrom) noexcept;
    GLRENDER_API Shader& operator=(Shader &&moveFrom) noexcept;
    
                  GLRENDER_API void use() const;
                  GLRENDER_API void setUniform(const std::string& name, float val);
                  GLRENDER_API void setUniform(const std::string& name, int32_t val);
                  GLRENDER_API void setUniform(const std::string& name, uint32_t val);
                  GLRENDER_API void setUniform(const std::string& name, vec2<float> val);
                  GLRENDER_API void setUniform(const std::string& name, vec3<float> val);
                  GLRENDER_API void setUniform(const std::string& name, vec4<float> val);
                  GLRENDER_API void setUniform(const std::string& name, const mat3x3<float>& val);
                  GLRENDER_API void setUniform(const std::string& name, const mat4x4<float>& val);
                  GLRENDER_API void sendUniforms() const;
    
    [[nodiscard]] GLRENDER_API bool exists() const;
                  GLRENDER_API void reset();
    
    uint32_t handle = std::numeric_limits<uint32_t>::max();
    
    private:
    std::unordered_map<std::string, Uniform> uniforms = {};
    bool init = false;
  };
}
