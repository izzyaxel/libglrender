#pragma once

#include "glrTexture.hh"
#include "glrShader.hh"
#include "glrMesh.hh"
#include "glrColor.hh"

#include "export.hh"

#include <memory>

namespace glr
{
  /// A bundle of information the renderer can use to draw something to a framebuffer
  struct Renderable
  {
    struct CharacterInfo
    {
      GLRENDER_API CharacterInfo()
      {
        this->m_character = '\0';
        this->m_color = {};
        this->m_atlasUVs = {};
      };
      
      GLRENDER_API CharacterInfo(char const &character, Color const &color, QuadUVs const &atlasUVs)
      {
        this->m_character = character;
        this->m_color = color;
        this->m_atlasUVs = atlasUVs;
      }
      
      char m_character = '\0';
      Color m_color = {};
      QuadUVs m_atlasUVs = {};
    };
    
    GLRENDER_API Renderable(vec2<double> const &pos,
      vec2<double> const &scale,
      double rotation,
      vec3<double> const &axis,
      std::shared_ptr<Texture> const &texture,
      std::shared_ptr<Shader> const &shader,
      std::shared_ptr<Mesh> const &mesh,
      size_t layer,
      size_t sublayer,
      std::string const &name,
      CharacterInfo characterInfo = CharacterInfo())
    {
      this->m_pos = pos;
      this->m_scale = scale;
      this->m_rotation = rotation;
      this->m_axis = axis;
      this->m_texture = texture;
      this->m_shader = shader;
      this->m_mesh = mesh;
      this->m_layer = layer;
      this->m_sublayer = sublayer;
      this->m_name = name;
      this->m_characterInfo = characterInfo;
    }
    
    vec2<double> m_pos = {};
    vec2<double> m_scale = {};
    double m_rotation = 0.0f;
    vec3<double> m_axis = {0.0f, 0.0f, 1.0f};
    std::shared_ptr<Texture> m_texture = nullptr;
    std::shared_ptr<Shader> m_shader = nullptr;
    std::shared_ptr<Mesh> m_mesh = nullptr;
    size_t m_layer = 0;
    size_t m_sublayer = 0;
    std::string m_name;
    CharacterInfo m_characterInfo;
  };
}
