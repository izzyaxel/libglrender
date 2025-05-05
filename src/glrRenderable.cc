#include "GLRender/glrRenderable.hh"

namespace glr
{
  const RenderableComps OBJECT_RENDERABLE_TEMPLATE{TRANSFORM, TEXTURE, MESH, FRAGVERTSHADER, LAYER};
  const RenderableComps TEXT_RENDERABLE_TEMPLATE{TRANSFORM, TEXTURE, MESH, FRAGVERTSHADER, LAYER, TEXT};
  const RenderableComps COMPUTE_RENDERABLE_TEMPLATE{COMPUTESHADER};
  
  RenderableID lastRenderableID = 0;

  std::unordered_map<uint64_t, std::shared_ptr<TransformComp>> transformComps{};
  std::unordered_map<uint64_t, std::shared_ptr<LayerComp>> layerComps{};
  std::unordered_map<uint64_t, std::shared_ptr<TextureComp>> textureComps{};
  std::unordered_map<uint64_t, std::shared_ptr<MeshComp>> meshComps{};
  std::unordered_map<uint64_t, std::shared_ptr<FragVertShaderComp>> fragvertComps{};
  std::unordered_map<uint64_t, std::shared_ptr<ComputeShaderComp>> computeComps{};
  std::unordered_map<uint64_t, std::shared_ptr<TextComp>> textComps{};
  
  CharInfo::CharInfo() {}

  CharInfo::CharInfo(const char& character, const Color& color, const QuadUVs& atlasUVs, const std::string& colorUniformLocation)
  {
    this->character = character;
    this->color = color;
    this->atlasUVs = atlasUVs;
    this->colorUniformLocation = colorUniformLocation;
  }

  bool CharInfo::operator==(const CharInfo& other) const
  {
    return this->character == other.character && this->color == other.color &&
    this->atlasUVs == other.atlasUVs && this->colorUniformLocation == other.colorUniformLocation;
  }

  RenderableID newRenderable(const RenderableComps& comps)
  {
    const RenderableID id = lastRenderableID;
    lastRenderableID++;
    for(const auto& comp : comps)
    {
      switch(comp)
      {
        case TRANSFORM:
        {
          transformComps[id] = std::make_shared<TransformComp>();
          break;
        }
        case LAYER:
        {
          layerComps[id] = std::make_shared<LayerComp>();
          break;
        }
        case TEXTURE:
        {
          textureComps[id] = std::make_shared<TextureComp>();
          break;
        }
        case MESH:
        {
          meshComps[id] = std::make_shared<MeshComp>();
          break;
        }
        case FRAGVERTSHADER:
        {
          fragvertComps[id] = std::make_shared<FragVertShaderComp>();
          break;
        }
        case COMPUTESHADER:
        {
          computeComps[id] = std::make_shared<ComputeShaderComp>();
          break;
        }
        case TEXT:
        {
          textComps[id] = std::make_shared<TextComp>();
          break;
        }
        default: break;
      }
    }
    return id;
  }

  std::shared_ptr<TransformComp> getTransformComp(const RenderableID id)
  {
    return transformComps.contains(id) ? transformComps.at(id) : nullptr;
  }
  
  std::shared_ptr<LayerComp> getLayerComp(const RenderableID id)
  {
    return layerComps.contains(id) ? layerComps.at(id) : nullptr;
  }

  std::shared_ptr<TextureComp> getTextureComp(const RenderableID id)
  {
    return textureComps.contains(id) ? textureComps.at(id) : nullptr;
  }

  std::shared_ptr<MeshComp> getMeshComp(const RenderableID id)
  {
    return meshComps.contains(id) ? meshComps.at(id) : nullptr;
  }
  
  std::shared_ptr<FragVertShaderComp> getFragVertComp(const RenderableID id)
  {
    return fragvertComps.contains(id) ? fragvertComps.at(id) : nullptr;
  }
  
  std::shared_ptr<ComputeShaderComp> getComputeComp(const RenderableID id)
  {
    return computeComps.contains(id) ? computeComps.at(id) : nullptr;
  }
  
  std::shared_ptr<TextComp> getTextComp(const RenderableID id)
  {
    return textComps.contains(id) ? textComps.at(id) : nullptr;
  }

  bool hasComp(const RenderableID id, const RenderableCompType comp)
  {
    switch(comp)
    {
      case TRANSFORM:
      {
        return transformComps.contains(id);
      }
      case LAYER:
      {
        return layerComps.contains(id);
      }
      case TEXTURE:
      {
        return textureComps.contains(id);
      }
      case MESH:
      {
        return meshComps.contains(id);
      }
      case FRAGVERTSHADER:
      {
        return fragvertComps.contains(id);
      }
      case COMPUTESHADER:
      {
        return computeComps.contains(id);
      }
      case TEXT:
      {
        return textComps.contains(id);
      }
      default: return false;
    }
  }

  bool isTemplate(const RenderableID id, const RenderableComps& tmplt)
  {
    bool out = true;
    for(const auto& comp : tmplt)
    {
      if(!hasComp(id, comp))
      {
        out = false;
        break;
      }
    }
    return out;
  }

  const std::unordered_map<uint64_t, std::shared_ptr<TransformComp>>& getTransformComps()
  {
    return transformComps;
  }
  
  const std::unordered_map<uint64_t, std::shared_ptr<LayerComp>>& getLayerComps()
  {
    return layerComps;
  }

  const std::unordered_map<uint64_t, std::shared_ptr<TextureComp>>& getTextureComps()
  {
    return textureComps;
  }

  const std::unordered_map<uint64_t, std::shared_ptr<MeshComp>>& getMeshComps()
  {
    return meshComps;
  }
  
  const std::unordered_map<uint64_t, std::shared_ptr<FragVertShaderComp>>& getFragvertComps()
  {
    return fragvertComps;
  }
  
  const std::unordered_map<uint64_t, std::shared_ptr<ComputeShaderComp>>& getComputeComps()
  {
    return computeComps;
  }
  
  const std::unordered_map<uint64_t, std::shared_ptr<TextComp>>& getTextComps()
  {
    return textComps;
  }
}
