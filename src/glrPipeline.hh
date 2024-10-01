#pragma once
#include "glrShader.hh"

namespace glr
{
  struct Pipeline
  {
    Pipeline() = default;
    explicit Pipeline(const std::vector<Shader*>& shaders);
    ~Pipeline();
    
    Pipeline(Pipeline&& moveFrom) noexcept;
    Pipeline &operator =(Pipeline&& moveFrom) noexcept;
    Pipeline(const Pipeline& other) = delete;
    Pipeline &operator =(const Pipeline& other) = delete;

    void use() const;
    void sendUniforms() const;

    private:
    bool init = false;
    uint32_t handle = std::numeric_limits<uint32_t>::max();
    std::vector<Shader*> shaders;
  };
}
