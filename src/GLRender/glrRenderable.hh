#pragma once

#include "export.hh"
#include "glrTexture.hh"
#include "glrShader.hh"
#include "glrMesh.hh"
#include "glrColor.hh"
#include "glrEnums.hh"

#include <commons/math/quaternion.hh>
#include <memory>

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
    std::shared_ptr<Texture> texture = nullptr;
  };
  
  struct MeshComp
  {
    std::shared_ptr<Mesh> mesh = nullptr;
  };
  
  struct FragVertShaderComp
  {
    std::shared_ptr<Shader> shader = nullptr;
  };

  struct ComputeShaderComp
  {
    std::unordered_map<int32_t, Texture*> imageBindings{};
    std::shared_ptr<Shader> shader = nullptr;
    IOMode ioMode{};
    GLColorFormat glColorFormat{};
  };

  struct TextComp
  {
    std::vector<CharInfo> characterInfo{};
  };
  
  typedef enum
  {
    TRANSFORM, LAYER, TEXT, TEXTURE, MESH, FRAGVERTSHADER, COMPUTESHADER,
  } RenderableCompType;
  
  using RenderableComps = std::vector<RenderableCompType>;
  
  GLRENDER_API extern const RenderableComps OBJECT_RENDERABLE_TEMPLATE;
  GLRENDER_API extern const RenderableComps TEXT_RENDERABLE_TEMPLATE;
  GLRENDER_API extern const RenderableComps COMPUTE_RENDERABLE_TEMPLATE;

  using RenderableID = uint64_t;

  GLRENDER_API RenderableID newRenderable(const RenderableComps& comps);
  GLRENDER_API void removeRenderable(RenderableID id);

  GLRENDER_API std::shared_ptr<TransformComp> getTransformComp(RenderableID id);
  GLRENDER_API std::shared_ptr<LayerComp> getLayerComp(RenderableID id);
  GLRENDER_API std::shared_ptr<TextureComp> getTextureComp(RenderableID id);
  GLRENDER_API std::shared_ptr<MeshComp> getMeshComp(RenderableID id);
  GLRENDER_API std::shared_ptr<FragVertShaderComp> getFragVertComp(RenderableID id);
  GLRENDER_API std::shared_ptr<ComputeShaderComp> getComputeComp(RenderableID id);
  GLRENDER_API std::shared_ptr<TextComp> getTextComp(RenderableID id);

  GLRENDER_API bool hasComp(RenderableID id, RenderableCompType comp);
  GLRENDER_API bool isTemplate(RenderableID id, const RenderableComps& tmplt);
  
  const std::unordered_map<uint64_t, std::shared_ptr<TransformComp>>& getTransformComps();
  const std::unordered_map<uint64_t, std::shared_ptr<LayerComp>>& getLayerComps();
  const std::unordered_map<uint64_t, std::shared_ptr<TextureComp>>& getTextureComps();
  const std::unordered_map<uint64_t, std::shared_ptr<MeshComp>>& getMeshComps();
  const std::unordered_map<uint64_t, std::shared_ptr<FragVertShaderComp>>& getFragvertComps();
  const std::unordered_map<uint64_t, std::shared_ptr<ComputeShaderComp>>& getComputeComps();
  const std::unordered_map<uint64_t, std::shared_ptr<TextComp>>& getTextComps();
}
