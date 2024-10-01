#pragma once
#include "glrShader.hh"

namespace glr
{
  /// A collection of shaders that are run in the order they're added to the pipeline
  struct ShaderPipeline
  {
    ShaderPipeline();
    ShaderPipeline(std::initializer_list<Shader*> shaders);
    ~ShaderPipeline();
    
    ShaderPipeline(ShaderPipeline&& moveFrom) noexcept;
    ShaderPipeline &operator =(ShaderPipeline&& moveFrom) noexcept;
    ShaderPipeline(const ShaderPipeline& other) = delete;
    ShaderPipeline &operator =(const ShaderPipeline& other) = delete;
    
    [[nodiscard]] GLRENDER_API bool isValid() const;
    [[nodiscard]] GLRENDER_API bool exists() const;
    void reset();
    
    /// Append a shader to the pipeline
    /// \param shader This structure does not own this pointer
    void append(Shader* shader);
    
    /// Bind this pipeline for use in rendering
    void use() const;
    
    /// Send all set uniforms to the GPU
    void sendUniforms() const;
    
    uint32_t handle = INVALID_HANDLE;
    
    private:
    bool init = false;
    std::vector<Shader*> shaders{};
  };
}
