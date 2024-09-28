#pragma once

#include "export.hh"

#include <variant>
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
      std::variant<float, int32_t, uint32_t, vec2<float>, vec3<float>, vec4<float>, mat3x3<float>, mat4x4<float>> val{};
    };
    
    Shader() = default;
    
    GLRENDER_API Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader);
    GLRENDER_API Shader(std::string const &name, std::string const &compShader);
    GLRENDER_API ~Shader();
    
    Shader(Shader const &copyFrom) = delete;
    Shader& operator=(Shader const &copyFrom) = delete;
    GLRENDER_API Shader(Shader &&moveFrom) noexcept;
    GLRENDER_API Shader& operator=(Shader &&moveFrom) noexcept;

    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use() const;
    GLRENDER_API void setUniform(const std::string& name, std::variant<float, int32_t, uint32_t, vec2<float>, vec3<float>, vec4<float>, mat3x3<float>, mat4x4<float>> val);
    GLRENDER_API void sendUniforms() const;
    
    uint32_t handle = std::numeric_limits<uint32_t>::max();
    
    private:
    std::unordered_map<std::string, Uniform> uniforms = {};
    bool init = false;
  };
}
