#pragma once

#include "export.hh"
#include "glrAssetID.hh"
#include "glrColor.hh"
#include "glrAtlas.hh"

#include <commons/math/quaternion.hh>
#include <memory>
#include <unordered_map>

namespace glr
{
  struct CharInfo
  {
    GLRENDER_API CharInfo();
    GLRENDER_API CharInfo(const char& character, const Color& color, const QuadUVs& atlasUVs, const std::string& colorUniformLocation);
    GLRENDER_API bool operator == (const CharInfo& other) const;
    
    char character = '\0';
    Color color = {};
    QuadUVs atlasUVs = {};
    std::string colorUniformLocation;
  };

  struct TransformComp
  {
    vec3<float> pos = {};
    vec3<float> scale = {};
    quat<float> rotation = {};
  };

  struct LayerComp
  {
    uint64_t layer = 0;
    uint64_t sublayer = 0;
  };

  struct TextureComp
  {
    ID texture = INVALID_ID;
    //std::shared_ptr<Texture> texture = nullptr;
  };
  
  struct MeshComp
  {
    ID mesh = INVALID_ID;
    //std::shared_ptr<Mesh> mesh = nullptr;
  };
  
  struct FragVertShaderComp
  {
    ID shader = INVALID_ID;
    //std::shared_ptr<Shader> shader = nullptr;
  };

  struct ComputeShaderComp
  {
    ID shader = INVALID_ID;
    //std::shared_ptr<Shader> shader = nullptr;
    std::unordered_map<int32_t, Texture*> imageBindings{};
    GLRIOMode ioMode{};
    GLRColorFormat glColorFormat{};
  };

  struct TextComp
  {
    std::vector<CharInfo> characterInfo{};
  };

  struct Renderable
  {
    std::shared_ptr<TransformComp> transformComp = nullptr;
    std::shared_ptr<LayerComp> layerComp = nullptr;
    std::shared_ptr<TextComp> textComp = nullptr;
    std::shared_ptr<TextureComp> textureComp = nullptr;
    std::shared_ptr<MeshComp> meshComp = nullptr;
    std::shared_ptr<FragVertShaderComp> fragVertShaderComp = nullptr;
    std::shared_ptr<ComputeShaderComp> computeShaderComp = nullptr;
  };
  
  typedef enum
  {
    TRANSFORM, LAYER, TEXT, TEXTURE, MESH, FRAGVERTSHADER, COMPUTESHADER,
  } RenderableCompType;
  
  using RenderableComps = std::vector<RenderableCompType>;
  
  GLRENDER_API extern const RenderableComps OBJECT_RENDERABLE_TEMPLATE;
  GLRENDER_API extern const RenderableComps TEXT_RENDERABLE_TEMPLATE;
  GLRENDER_API extern const RenderableComps COMPUTE_RENDERABLE_TEMPLATE;
  
  GLRENDER_API Renderable newRenderable(const RenderableComps& comps);
  GLRENDER_API bool isTemplate(const Renderable& renderable, const RenderableComps& tmplt);
}
