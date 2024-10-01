#include "glrPipeline.hh"

#include "glad/gl.hh"

namespace glr
{
  Pipeline::Pipeline(std::initializer_list<Shader*> shaders)
  {
    glGenProgramPipelines(1, &this->handle);
    for(const auto& shader : shaders)
    {
      if(shader->type == Shader::FRAGVERT)
      {
        glUseProgramStages(GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, 1, shader->handle);
      }
      else if(shader->type == Shader::COMP)
      {
        glUseProgramStages(GL_COMPUTE_SHADER_BIT, 1, shader->handle);
      }
      this->shaders.emplace_back(shader);
    }
    this->init = true;
  }

  Pipeline::~Pipeline()
  {
    glDeleteProgramPipelines(1, &this->handle);
  }
  
  Pipeline::Pipeline(Pipeline&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->shaders = std::move(moveFrom.shaders);
    moveFrom.shaders = {};
    
    this->init = true;
    moveFrom.init = false;
  }

  Pipeline& Pipeline::operator=(Pipeline&& moveFrom) noexcept
  {
    this->handle = moveFrom.handle;
    moveFrom.handle = std::numeric_limits<uint32_t>::max();
    
    this->shaders = std::move(moveFrom.shaders);
    moveFrom.shaders = {};
    
    this->init = true;
    moveFrom.init = false;

    return *this;
  }

  void Pipeline::use() const
  {
    glUseProgram(0);
    glBindProgramPipeline(this->handle);
  }
  
  void Pipeline::sendUniforms() const
  {
    for(const auto& shader : this->shaders)
    {
      shader->sendUniforms();
    }
  }
}
