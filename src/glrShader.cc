#include "glrShader.hh"

#include <glad/gl.hh>

namespace glr
{
  Shader::Shader(const std::string& name, const std::string& vertShader, const std::string& fragShader)
  {
    const uint32_t vertHandle = glCreateShader(GL_VERTEX_SHADER);
    const uint32_t fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    this->handle = glCreateProgram();
    const char* vertSource = vertShader.data();
    const char* fragSource = fragShader.data();
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
      const std::string errorStr{error.begin(), error.end()};
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
      const std::string errorStr{error.begin(), error.end()};
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
    this->type = FRAGVERT;
  }
  
  Shader::Shader(const std::string& name, const std::string& compShader)
  {
    const uint32_t compHandle = glCreateShader(GL_COMPUTE_SHADER);
    this->handle = glCreateProgram();
    const char* compSource = compShader.data();
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
      const std::string errorStr{error.begin(), error.end()};
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
      const std::string errorStr{error.begin(), error.end()};
      printf("Compute shader program error: %s failed to link: %s\n", name.c_str(), errorStr.c_str());
      return;
    }
    glDetachShader(this->handle, compHandle);
    glDeleteShader(compHandle);
    this->init = true;
    this->type = COMP;
  }
  
  Shader::~Shader()
  {
    glDeleteProgram(this->handle);
  }
  
  Shader::Shader(Shader&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = INVALID_HANDLE;
    
    this->type = moveFrom.type;
    moveFrom.type = INVALID;
    
    this->uniforms = std::move(moveFrom.uniforms);
    moveFrom.uniforms = {};
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Shader& Shader::operator=(Shader&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = INVALID_HANDLE;
    
    this->type = moveFrom.type;
    moveFrom.type = INVALID;
    
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
        glProgramUniform1f(this->handle, handle, std::get<float>(val));
      }
      else if(std::holds_alternative<int32_t>(val))
      {
        glProgramUniform1i(this->handle, handle, std::get<int32_t>(val));
      }
      else if(std::holds_alternative<uint32_t>(val))
      {
        glProgramUniform1ui(this->handle, handle, std::get<uint32_t>(val));
      }
      else if(std::holds_alternative<vec2<float>>(val))
      {
        glProgramUniform2fv(this->handle, handle, 1, std::get<vec2<float>>(val).data);
      }
      else if(std::holds_alternative<vec3<float>>(val))
      {
        glProgramUniform3fv(this->handle, handle, 1, std::get<vec3<float>>(val).data);
      }
      else if(std::holds_alternative<vec4<float>>(val))
      {
        glProgramUniform4fv(this->handle, handle, 1, std::get<vec4<float>>(val).data);
      }
      else if(std::holds_alternative<mat3x3<float>>(val))
      {
        glProgramUniformMatrix3fv(this->handle, handle, 1, GL_FALSE, &std::get<mat3x3<float>>(val).data[0][0]);
      }
      else if(std::holds_alternative<mat4x4<float>>(val))
      {
        glProgramUniformMatrix4fv(this->handle, handle, 1, GL_FALSE, &std::get<mat4x4<float>>(val).data[0][0]);
      }
    }
  }
  
  bool Shader::isValid() const
  {
    return this->init && this->handle != INVALID_HANDLE;
  }
  
  [[maybe_unused]] bool Shader::exists() const
  {
    return this->init;
  }
  
  void Shader::reset()
  {
    glDeleteProgram(this->handle);
    this->handle = INVALID_HANDLE;
    this->uniforms = {};
    this->init = false;
  }
}
