#include "GLRender/glrRenderable.hh"

namespace glr
{
  const RenderableComps OBJECT_RENDERABLE_TEMPLATE{TRANSFORM, TEXTURE, MESH, FRAGVERTSHADER, LAYER};
  const RenderableComps TEXT_RENDERABLE_TEMPLATE{TRANSFORM, TEXTURE, MESH, FRAGVERTSHADER, LAYER, TEXT};
  const RenderableComps COMPUTE_RENDERABLE_TEMPLATE{COMPUTESHADER};
  
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

  Renderable newRenderable(const RenderableComps& comps)
  {
    Renderable out;
    for(const auto& comp : comps)
    {
      switch(comp)
      {
        case TRANSFORM:
        {
          out.transformComp = std::make_shared<TransformComp>();
          break;
        }
        case LAYER:
        {
          out.layerComp = std::make_shared<LayerComp>();
          break;
        }
        case TEXTURE:
        {
          out.textureComp = std::make_shared<TextureComp>();
          break;
        }
        case MESH:
        {
          out.meshComp = std::make_shared<MeshComp>();
          break;
        }
        case FRAGVERTSHADER:
        {
          out.fragVertShaderComp = std::make_shared<FragVertShaderComp>();
          break;
        }
        case COMPUTESHADER:
        {
          out.computeShaderComp = std::make_shared<ComputeShaderComp>();
          break;
        }
        case TEXT:
        {
          out.textComp = std::make_shared<TextComp>();
          break;
        }
        default: break;
      }
    }
    return std::move(out);
  }

  bool isTemplate(const Renderable& renderable, const RenderableComps& tmplt)
  {
    for(const auto& comp : tmplt)
    {
      switch(comp)
      {
        case TRANSFORM:
        {
          if(!renderable.transformComp)
          {
            return false;
          }
          break;
        }
        case LAYER:
        {
          if(!renderable.layerComp)
          {
            return false;
          }
          break;
        }
        case TEXT:
        {
          if(!renderable.textComp)
          {
            return false;
          }
          break;
        }
        case TEXTURE:
        {
          if(!renderable.textureComp)
          {
            return false;
          }
          break;
        }
        case MESH:
        {
          if(!renderable.meshComp)
          {
            return false;
          }
          break;
        }
        case FRAGVERTSHADER:
        {
          if(!renderable.fragVertShaderComp)
          {
            return false;
          }
          break;
        }
        case COMPUTESHADER:
        {
          if(!renderable.computeShaderComp)
          {
            return false;
          }
          break;
        }
        default: break;
      }
    }
    return true;
  }
}
