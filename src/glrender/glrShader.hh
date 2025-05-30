#pragma once

#include "export.hh"
#include "glrUtil.hh"

#include <commons/math/vec2.hh>
#include <commons/math/vec3.hh>
#include <commons/math/vec4.hh>
#include <commons/math/mat3.hh>
#include <commons/math/mat4.hh>

#include <variant>
#include <string>
#include <unordered_map>

#include "glrEnums.hh"

namespace glr
{
  /// An OpenGL frag/vert, or compute shader
  struct Shader
  {
    using UniformValue = std::variant<float, int32_t, uint32_t, vec2<uint32_t>, vec2<int32_t>, vec2<float>, vec3<uint32_t>, vec3<int32_t>, vec3<float>, vec4<uint32_t>, vec4<int32_t>, vec4<float>, mat3x3<float>, mat4x4<float>>;
    
    struct Uniform
    {
      int32_t handle = std::numeric_limits<int32_t>::max();
      UniformValue val{};
    };
    
    Shader() = default;
    
    GLRENDER_API Shader(const std::string& name, const std::string& vertShader, const std::string& fragShader);
    GLRENDER_API Shader(const std::string& name, const std::string& compShader);
    GLRENDER_API ~Shader();
    
    Shader(const Shader& copyFrom) = delete;
    Shader& operator=(const Shader& copyFrom) = delete;
    GLRENDER_API Shader(Shader&& moveFrom) noexcept;
    GLRENDER_API Shader& operator=(Shader&& moveFrom) noexcept;
    
    GLRENDER_API bool isValid() const;
    GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use() const;
    GLRENDER_API void setUniform(const std::string& name, UniformValue val);
    GLRENDER_API void sendUniforms() const;
    
    uint32_t handle = INVALID_HANDLE;
    
    GLRShaderType type = GLRShaderType::INVALID;
    
    private:
    std::unordered_map<std::string, Uniform> uniforms = {};
    bool init = false;
  };
}
