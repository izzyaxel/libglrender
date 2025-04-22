#pragma once

#include "glrTexture.hh"
#include "glrShader.hh"
#include "glrMesh.hh"
#include "glrColor.hh"

#include "export.hh"

#include <commons/math/quaternion.hh>

namespace glr
{
  typedef enum
  {
    NORMAL, COMPUTE
  } RenderableType;
  
  /// A bundle of information the renderer can use to draw something to a framebuffer
  struct Renderable
  {
    struct CharacterInfo
    {
      GLRENDER_API CharacterInfo(){}
      
      GLRENDER_API CharacterInfo(const char& character, const Color& color, const QuadUVs& atlasUVs, const std::string& colorUniformLocation)
      {
        this->character = character;
        this->color = color;
        this->atlasUVs = atlasUVs;
        this->colorUniformLocation = colorUniformLocation;
      }
      
      bool operator==(const CharacterInfo& other) const
      {
        return this->character == other.character && this->color == other.color &&
        this->atlasUVs == other.atlasUVs && this->colorUniformLocation == other.colorUniformLocation;
      }
      
      char character = '\0';
      Color color = {};
      QuadUVs atlasUVs = {};
      std::string colorUniformLocation;
    };
    
    GLRENDER_API Renderable(RenderableType type,
                            vec3<float> const &pos,
                            const vec3<float>& scale,
                            const quat<float>& rotation,
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
                            texture(texture),
                            shader(shader),
                            mesh(mesh),
                            layer(layer),
                            sublayer(sublayer),
                            name(std::move(name)),
                            characterInfo(std::move(characterInfo)) {}
    
    bool operator==(const Renderable& other) const
    {
      return this->pos == other.pos && this->scale == other.scale && this->rotation == other.rotation && this->texture == other.texture &&
             this->shader == other.shader && this->mesh == other.mesh && this->layer == other.layer &&
             this->sublayer == other.sublayer && this->name == other.name && this->characterInfo == other.characterInfo;
    }

    RenderableType type = NORMAL;
    vec3<float> pos = {};
    vec3<float> scale = {};
    quat<float> rotation = {};
    Texture* texture = nullptr;
    Shader* shader = nullptr;
    Mesh* mesh = nullptr;
    size_t layer = 0;
    size_t sublayer = 0;
    std::string name;
    CharacterInfo characterInfo{};
  };
}
