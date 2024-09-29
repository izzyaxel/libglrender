#pragma once

#include "glrTexture.hh"
#include "glrShader.hh"
#include "glrMesh.hh"
#include "glrColor.hh"

#include "export.hh"

namespace glr
{
  /// A bundle of information the renderer can use to draw something to a framebuffer
  struct Renderable
  {
    struct CharacterInfo
    {
      GLRENDER_API CharacterInfo()
      {
        this->character = '\0';
        this->color = {};
        this->atlasUVs = {};
      };
      
      GLRENDER_API CharacterInfo(const char& character, const Color& color, const QuadUVs& atlasUVs)
      {
        this->character = character;
        this->color = color;
        this->atlasUVs = atlasUVs;
      }
      
      char character = '\0';
      Color color = {};
      QuadUVs atlasUVs = {};
    };
    
    GLRENDER_API Renderable(vec2<float> const &pos,
      const vec2<float>& scale,
      const float rotation,
      const vec3<float>& axis,
      Texture *texture,
      Shader *shader,
      Mesh *mesh,
      const size_t layer,
      const size_t sublayer,
      std::string name,
      CharacterInfo characterInfo = CharacterInfo()) :
      pos(pos),
      scale(scale),
      rotation(rotation),
      axis(axis),
      texture(texture),
      shader(shader),
      mesh(mesh),
      layer(layer),
      sublayer(sublayer),
      name(std::move(name)),
      characterInfo(std::move(characterInfo))
    {}
    
    vec2<float> pos = {};
    vec2<float> scale = {};
    float rotation = 0.0f;
    vec3<float> axis = {0.0f, 0.0f, 1.0f};
    Texture* texture = nullptr;
    Shader* shader = nullptr;
    Mesh* mesh = nullptr;
    size_t layer = 0;
    size_t sublayer = 0;
    std::string name;
    CharacterInfo characterInfo{};
  };
}
