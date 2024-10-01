#include "glrShaderPipeline.hh"

#include "glad/gl.hh"

namespace glr
{
  ShaderPipeline::ShaderPipeline()
  {
    glGenProgramPipelines(1, &this->handle);
    this->init = true;
  }
  
  ShaderPipeline::ShaderPipeline(std::initializer_list<Shader*> shaders)
  {
    glGenProgramPipelines(1, &this->handle);
    for(auto& shader : shaders)
    {
      this->append(shader);
    }
    this->init = true;
  }

  ShaderPipeline::~ShaderPipeline()
  {
    glDeleteProgramPipelines(1, &this->handle);
  }
  
  ShaderPipeline::ShaderPipeline(ShaderPipeline&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = INVALID_HANDLE;
    
    this->shaders = std::move(moveFrom.shaders);
    moveFrom.shaders = {};
    
    this->init = true;
    moveFrom.init = false;
  }

  ShaderPipeline& ShaderPipeline::operator=(ShaderPipeline&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = INVALID_HANDLE;
    
    this->shaders = std::move(moveFrom.shaders);
    moveFrom.shaders = {};
    
    this->init = true;
    moveFrom.init = false;

    return *this;
  }
  
  void ShaderPipeline::append(Shader *shader)
  {
    if(shader->type == Shader::FRAGVERT)
    {
      glUseProgramStages(GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, 1, shader->handle);
    }
    else if(shader->type == Shader::COMP)
    {
      glUseProgramStages(GL_COMPUTE_SHADER_BIT, 1, shader->handle);
    }
    else if(shader->type == Shader::GEOMFRAG)
    {
      glUseProgramStages(GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, 1, shader->handle);
    }
    this->shaders.emplace_back(shader);
  }
  
  bool ShaderPipeline::isValid() const
  {
    return this->init && this->handle != INVALID_HANDLE;
  }
  
  bool ShaderPipeline::exists() const
  {
    return this->init;
  }
  
  void ShaderPipeline::reset()
  {
    glDeleteProgramPipelines(1, &this->handle);
    this->handle = INVALID_HANDLE;
    this->shaders.clear();
    this->init = false;
  }

  void ShaderPipeline::use() const
  {
    glUseProgram(0);
    glBindProgramPipeline(this->handle);
  }
  
  void ShaderPipeline::sendUniforms() const
  {
    for(const auto& shader : this->shaders)
    {
      shader->sendUniforms();
    }
  }
}
