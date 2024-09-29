#include "glrPipeline.hh"

#include "glad/gl.hh"

namespace glr
{
  Pipeline::Pipeline(const std::initializer_list<Shader*> shaders)
  {
    glGenProgramPipelines(1, &this->handle);
    //TODO need shader programs, do I want to store them in Shader and get them, or take shader sources here and generate them
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
}
