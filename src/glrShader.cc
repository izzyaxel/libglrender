#include "glrShader.hh"

#include <glad/gl.hh>

namespace glr
{
  Shader::Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader)
  {
    const uint32_t vertHandle = glCreateShader(GL_VERTEX_SHADER), fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    this->handle = glCreateProgram();
    char const *vertSource = vertShader.data(), *fragSource = fragShader.data();
    glShaderSource(vertHandle, 1, &vertSource, nullptr);
    glShaderSource(fragHandle, 1, &fragSource, nullptr);
    glCompileShader(vertHandle);
    int32_t success = 0;
    glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
      int32_t maxLen = 0;
      glGetShaderiv(vertHandle, GL_INFO_LOG_LENGTH, &maxLen);
      std::vector<char> error;
      error.resize(maxLen * sizeof(GLchar));
      glGetShaderInfoLog(vertHandle, maxLen, &maxLen, error.data());
      std::string errorStr{error.begin(), error.end()};
      printf("Vert/Frag shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
      return;
    }
    glCompileShader(fragHandle);
    success = 0;
    glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
      int32_t maxLen = 0;
      glGetShaderiv(fragHandle, GL_INFO_LOG_LENGTH, &maxLen);
      std::vector<char> error;
      error.resize(maxLen * sizeof(GLchar));
      glGetShaderInfoLog(fragHandle, maxLen, &maxLen, error.data());
      std::string errorStr{error.begin(), error.end()};
      printf("Vert/Frag shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
      return;
    }
    glAttachShader(this->handle, vertHandle);
    glAttachShader(this->handle, fragHandle);
    glLinkProgram(this->handle);
    success = 0;
    glGetProgramiv(this->handle, GL_LINK_STATUS, &success);
    if(!success)
    {
      int32_t maxLen = 0;
      glGetShaderiv(this->handle, GL_INFO_LOG_LENGTH, &maxLen);
      std::vector<char> error;
      error.resize(maxLen * sizeof(GLchar));
      glGetShaderInfoLog(this->handle, maxLen, &maxLen, error.data());
      std::string errorStr{error.begin(), error.end()};
      printf("Vert/Frag shader program error: %s failed to link\n", name.c_str());
      return;
    }
    glDetachShader(this->handle, vertHandle);
    glDetachShader(this->handle, fragHandle);
    glDeleteShader(vertHandle);
    glDeleteShader(fragHandle);
    this->init = true;
  }
  
  Shader::Shader(std::string const &name, std::string const &compShader)
  {
    const uint32_t compHandle = glCreateShader(GL_COMPUTE_SHADER);
    this->handle = glCreateProgram();
    char const *compSource = compShader.data();
    glShaderSource(compHandle, 1, &compSource, nullptr);
    glCompileShader(compHandle);
    int32_t success = 0;
    glGetShaderiv(compHandle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
      int32_t maxLen = 0;
      glGetShaderiv(compHandle, GL_INFO_LOG_LENGTH, &maxLen);
      std::vector<char> error;
      error.resize(maxLen * sizeof(GLchar));
      glGetShaderInfoLog(compHandle, maxLen, &maxLen, error.data());
      std::string errorStr{error.begin(), error.end()};
      printf("Compute shader error: %s failed to compile: %s\n", name.c_str(), errorStr.c_str());
      return;
    }
    
    glAttachShader(this->handle, compHandle);
    glLinkProgram(this->handle);
    success = 0;
    glGetProgramiv(this->handle, GL_LINK_STATUS, &success);
    if(!success)
    {
      int32_t maxLen = 0;
      glGetShaderiv(this->handle, GL_INFO_LOG_LENGTH, &maxLen);
      std::vector<char> error;
      error.resize(maxLen * sizeof(GLchar));
      glGetShaderInfoLog(this->handle, maxLen, &maxLen, error.data());
      std::string errorStr{error.begin(), error.end()};
      printf("Compute shader program error: %s failed to link: %s\n", name.c_str(), errorStr.c_str());
      return;
    }
    glDetachShader(this->handle, compHandle);
    glDeleteShader(compHandle);
    this->init = true;
  }
  
  Shader::~Shader()
  {
    glDeleteProgram(this->handle);
  }
  
  Shader::Shader(Shader &&moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->uniforms = std::move(moveFrom.uniforms);
    moveFrom.uniforms = {};
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Shader& Shader::operator=(Shader &&moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->uniforms = std::move(moveFrom.uniforms);
    moveFrom.uniforms = {};
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  void Shader::use() const
  {
    glUseProgram(this->handle);
  }
  
  void makeUniform(const std::string& location, const uint32_t shaderHandle, std::unordered_map<std::string, Shader::Uniform>& uniforms)
  {
    if(!uniforms.contains(location))
    {
      uniforms[location] = {};
      uniforms.at(location).handle = glGetUniformLocation(shaderHandle, location.data());
    }
  }

  void Shader::setUniform(const std::string& name, UniformValue val)
  {
    makeUniform(name, this->handle, this->uniforms);
    this->uniforms.at(name).val = std::move(val);
  }

  void Shader::sendUniforms() const
  {
    for(const auto& pair : this->uniforms)
    {
      const auto& [handle, val] = pair.second;
      if(std::holds_alternative<float>(val))
      {
        glUniform1f(handle, std::get<float>(val));
      }
      else if(std::holds_alternative<int32_t>(val))
      {
        glUniform1i(handle, std::get<int32_t>(val));
      }
      else if(std::holds_alternative<uint32_t>(val))
      {
        glUniform1ui(handle, std::get<uint32_t>(val));
      }
      else if(std::holds_alternative<vec2<float>>(val))
      {
        glUniform2fv(handle, 1, std::get<vec2<float>>(val).data);
      }
      else if(std::holds_alternative<vec3<float>>(val))
      {
        glUniform3fv(handle, 1, std::get<vec3<float>>(val).data);
      }
      else if(std::holds_alternative<vec4<float>>(val))
      {
        glUniform4fv(handle, 1, std::get<vec4<float>>(val).data);
      }
      else if(std::holds_alternative<mat3x3<float>>(val))
      {
        glUniformMatrix3fv(handle, 1, GL_FALSE, &std::get<mat3x3<float>>(val).data[0][0]);
      }
      else if(std::holds_alternative<mat4x4<float>>(val))
      {
        glUniformMatrix4fv(handle, 1, GL_FALSE, &std::get<mat4x4<float>>(val).data[0][0]);
      }
    }
  }
  
  bool Shader::exists() const
  {
    return this->init;
  }
  
  void Shader::reset()
  {
    glDeleteProgram(this->handle);
    this->handle = std::numeric_limits<uint32_t>::max();
    this->uniforms = {};
    this->init = false;
  }
}
