#include "glrShader.hh"

#include <glad/gl.hh>

namespace glr
{
  /*Shader::Shader(std::string const &name, std::vector<uint8_t> const &vertShader, std::vector<uint8_t> const &fragShader) : Shader(name, std::string{vertShader.begin(), vertShader.end()}, std::string{fragShader.begin(), fragShader.end()})
  {
    
  }
  
  Shader::Shader(std::string const &name, std::vector<uint8_t> const &compShader) : Shader(name, std::string{compShader.begin(), compShader.end()})
  {
    
  }*/
  
  Shader::Shader(std::string const &name, std::string const &vertShader, std::string const &fragShader)
  {
    uint32_t vertHandle = glCreateShader(GL_VERTEX_SHADER), fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
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
    uint32_t compHandle = glCreateShader(GL_COMPUTE_SHADER);
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
      uniforms.insert({location, {}});
      uniforms.at(location).handle = glGetUniformLocation(shaderHandle, location.data());
    }
  }
  
  void Shader::setUniform(const std::string& name, const float val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::FLOAT;
    uniform.valF = val;
  }

  void Shader::setUniform(const std::string& name, const int32_t val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::INT;
    uniform.valI = val;
  }

  void Shader::setUniform(const std::string& name, const uint32_t val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::UINT;
    uniform.valUI = val;
  }

  void Shader::setUniform(const std::string& name, const vec2<float> val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::VEC2;
    uniform.valVec2 = val;
  }

  void Shader::setUniform(const std::string& name, const vec3<float> val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::VEC3;
    uniform.valVec3 = val;
  }

  void Shader::setUniform(const std::string& name, const vec4<float> val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::VEC4;
    uniform.valVec4 = val;
  }

  void Shader::setUniform(const std::string& name, const mat3x3<float>& val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::MAT3;
    uniform.valMat3 = val;
  }

  void Shader::setUniform(const std::string& name, const mat4x4<float>& val)
  {
    makeUniform(name, this->handle, this->uniforms);
    Uniform& uniform = this->uniforms.at(name);
    uniform.type = Uniform::MAT4;
    uniform.valMat4 = val;
  }

  void Shader::sendUniforms() const
  {
    for(const auto& pair : this->uniforms)
    {
      const Uniform& uniform = pair.second;
      switch(uniform.type)
      {
        case Uniform::FLOAT:
        {
          glUniform1f(uniform.handle, uniform.valF);
          break;
        }
        case Uniform::INT:
        {
          glUniform1i(uniform.handle, uniform.valI);
          break;
        }
        case Uniform::UINT:
        {
          glUniform1ui(uniform.handle, uniform.valUI);
          break;
        }
        case Uniform::VEC2:
        {
          glUniform2fv(uniform.handle, 1, uniform.valVec2.data);
          break;
        }
        case Uniform::VEC3:
        {
          glUniform3fv(uniform.handle, 1, uniform.valVec3.data);
          break;
        }
        case Uniform::VEC4:
        {
          glUniform4fv(uniform.handle, 1, uniform.valVec4.data);
          break;
        }
        case Uniform::MAT3:
        {
          glUniformMatrix3fv(uniform.handle, 1, GL_FALSE, &uniform.valMat3.data[0][0]);
          break;
        }
        case Uniform::MAT4:
        {
          glUniformMatrix4fv(uniform.handle, 1, GL_FALSE, &uniform.valMat4.data[0][0]);
          break;
        }
        case Uniform::INVALID: break;
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
