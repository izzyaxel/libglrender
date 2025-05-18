#include "glrender/glrAssetRepository.hh"

#include <memory>
#include <unordered_map>

#include "glad/gl.hh"

namespace glr::asset_repo
{
  std::unordered_map<ID, std::shared_ptr<Shader>> shaders{};
  std::unordered_map<ID, std::shared_ptr<Texture>> textures{};
  std::unordered_map<ID, std::shared_ptr<Mesh>> meshes{};
  std::unordered_map<ID, std::shared_ptr<Framebuffer>> fbos{};
  std::unordered_map<ID, std::shared_ptr<Atlas>> atlases{};
  std::unordered_map<ID, std::shared_ptr<ShaderPipeline>> shaderPipelines{};

  ID lastShader = 0;
  ID lastTexture = 0;
  ID lastMesh = 0;
  ID lastFBO = 0;
  ID lastAtlas = 0;
  ID lastShaderPipeline = 0;

  //Management
  ID newShader(const std::string& shaderName, const std::string& vertSrc, const std::string& fragSrc)
  {
    const ID out = lastShader;
    lastShader++;
    shaders[out] = std::make_shared<Shader>(shaderName, vertSrc, fragSrc);
    return out;
  }
  
  ID newShader(const std::string& shaderName, const std::string& compSrc)
  {
    const ID out = lastShader;
    lastShader++;
    shaders[out] = std::make_shared<Shader>(shaderName, compSrc);
    return out;
  }

  ID newTexture(const std::string& textureName, const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t channels, const GLRFilterMode min, const GLRFilterMode mag, const bool sRGB)
  {
    const ID out = lastTexture;
    lastTexture++;
    textures[out] = std::make_shared<Texture>(textureName, data, width, height, channels, min, mag, sRGB);
    return out;
  }
  
  ID newMesh()
  {
    const ID out = lastMesh;
    lastMesh++;
    meshes[out] = std::make_shared<Mesh>();
    return out;
  }
  
  ID newFBO()
  {
    const ID out = lastFBO;
    lastFBO++;
    fbos[out] = std::make_shared<Framebuffer>();
    return out;
  }

  ID newAtlas()
  {
    const ID out = lastAtlas;
    lastAtlas++;
    atlases[out] = std::make_shared<Atlas>();
    return out;
  }
  
  ID newShaderPipeline()
  {
    const ID out = lastShaderPipeline;
    lastShaderPipeline++;
    shaderPipelines[out] = std::make_shared<ShaderPipeline>();
    return out;
  }

  bool shaderExists(const ID shader)
  {
    return shaders.contains(shader);
  }
  
  bool textureExists(const ID texture)
  {
    return textures.contains(texture);
  }
  
  bool meshExists(const ID mesh)
  {
    return meshes.contains(mesh);
  }
  
  bool fboExists(const ID fbo)
  {
    return fbos.contains(fbo);
  }
  
  bool atlasExists(const ID atlas)
  {
    return atlases.contains(atlas);
  }
  
  bool shaderPipelineExists(const ID shaderPipeline)
  {
    return shaderPipelines.contains(shaderPipeline);
  }
  
  void deleteShader(const ID shader)
  {
    if(shaders.contains(shader))
    {
      shaders.erase(shader);
    }
  }
  
  void deleteTexture(const ID texture)
  {
    if(textures.contains(texture))
    {
      textures.erase(texture);
    }
  }
  
  void deleteMesh(const ID mesh)
  {
    if(meshes.contains(mesh))
    {
      meshes.erase(mesh);
    }
  }
  
  void deleteFBO(const ID fbo)
  {
    if(fbos.contains(fbo))
    {
      fbos.erase(fbo);
    }
  }

  void deleteAtlas(const ID atlas)
  {
    if(atlases.contains(atlas))
    {
      atlases.erase(atlas);
    }
  }

  void deleteShaderPipeline(const ID shaderPipeline)
  {
    if(shaderPipelines.contains(shaderPipeline))
    {
      shaderPipelines.erase(shaderPipeline);
    }
  }

  void deleteShaders()
  {
    shaders.clear();
  }
  
  void deleteTextures()
  {
    textures.clear();
  }
  
  void deleteMeshes()
  {
    meshes.clear();
  }
  
  void deleteFBOs()
  {
    fbos.clear();
  }
  
  void deleteAtlases()
  {
    atlases.clear();
  }
  
  void deleteShaderPipelines()
  {
    shaderPipelines.clear();
  }
  
  void deleteAll()
  {
    deleteShaders();
    deleteTextures();
    deleteMeshes();
    deleteFBOs();
    deleteAtlases();
    deleteShaderPipelines();
  }


  //Method forwarding
  //Shader
  void shaderUse(const ID shader)
  {
    if(!shaders.contains(shader))
    {
      return;
    }
    shaders.at(shader)->use();
  }
  
  void shaderSetUniform(const ID shader, const std::string& name, const Shader::UniformValue& val)
  {
    if(!shaders.contains(shader))
    {
      return;
    }
    shaders.at(shader)->setUniform(name, val);
  }
  
  void shaderSendUniforms(const ID shader)
  {
    if(!shaders.contains(shader))
    {
      return;
    }
    shaders.at(shader)->sendUniforms();
  }

  //Texture
  void textureUse(const ID texture)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->use();
  }

  void textureUseAsImage(const ID texture, const uint32_t target, const GLRIOMode mode, const GLRColorFormat format)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    glBindImageTexture(target, textures.at(texture)->handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
  }

  void textureSetBindingTarget(const ID texture, const uint32_t target)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->bindingIndex = target;
  }
  
  void textureSetFilterMode(const ID texture, const GLRFilterMode min, const GLRFilterMode mag)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->setFilterMode(min, mag);
  }
  
  void textureSetAnisotropyLevel(const ID texture, const uint32_t level)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->setAnisotropyLevel(level);
  }
  
  void textureSubImage(const ID texture, const uint8_t* data, const uint32_t width, const uint32_t height, const uint32_t xPos, const uint32_t yPos, const uint8_t channels)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->subImage(data, width, height, xPos, yPos, channels);
  }
  
  void textureClear(const ID texture)
  {
    if(!textures.contains(texture))
    {
      return;
    }
    textures.at(texture)->clear();
  }
  
  DownloadedImageData textureDownload(const ID texture, const uint8_t channels)
  {
    if(!textures.contains(texture))
    {
      return {};
    }
    return textures.at(texture)->downloadTexture(channels);
  }

  //Mesh
  void meshUse(const ID mesh)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->use();
  }

  void meshSetPositionDimensions(const ID mesh, const GLRDimensions dimensions)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->setPositionDimensions(dimensions);
  }
  
  void meshAddPositions(const ID mesh, const float* positions, const size_t positionsSize, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->addPositions(positions, positionsSize, callback);
  }
  
  void meshAddIndices(const ID mesh, const uint32_t* indices, const size_t indicesSize, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->addIndices(indices, indicesSize, callback);
  }
  
  void meshAddUVs(const ID mesh, const float* uvs, const size_t uvsSize, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->addUVs(uvs, uvsSize, callback);
  }
  
  void meshAddNormals(const ID mesh, const float* normals, const size_t normalsSize, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->addNormals(normals, normalsSize, callback);
  }
  
  void meshAddColors(const ID mesh, const float* colors, const size_t colorsSize, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->addColors(colors, colorsSize, callback);
  }

  size_t meshGetVertices(ID mesh)
  {
    if(!meshes.contains(mesh))
    {
      return 0;
    }
    return meshes.at(mesh)->numVerts;
  }
  
  size_t meshGetIndices(ID mesh)
  {
    if(!meshes.contains(mesh))
    {
      return 0;
    }
    return meshes.at(mesh)->numIndices;
  }
  
  void meshFinalize(const ID mesh, const LoggingCallback& callback)
  {
    if(!meshes.contains(mesh))
    {
      return;
    }
    meshes.at(mesh)->finalize(callback);
  }
  
  bool meshIsFinalized(const ID mesh)
  {
    if(!meshes.contains(mesh))
    {
      return true;
    }
    return meshes.at(mesh)->isFinalized();
  }
  
  bool meshIsIndexed(const ID mesh)
  {
    if(!meshes.contains(mesh))
    {
      return false;
    }
    return meshes.at(mesh)->isIndexed();
  }

  //Framebuffer
  void fboUse(const ID fbo)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->use();
  }
  
  void fboSetDimensions(const ID fbo, const uint32_t width, const uint32_t height)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->setDimensions(width, height);
  }
  
  void fboAddColorAttachment(const ID fbo, const GLRAttachmentType attachmentType, const uint8_t channels)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->addColorAttachment(attachmentType, channels);
  }
  
  void fboAddDepthAttachment(const ID fbo, const GLRAttachmentType attachmentType)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->addDepthAttachment(attachmentType);
  }
  
  void fboAddStencilAttachment(const ID fbo, const GLRAttachmentType attachmentType)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->addStencilAttachment(attachmentType);
  }
  
  void fboFinalize(const ID fbo)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->finalize();
  }
  
  void fboBindAttachment(const ID fbo, const GLRAttachment attachment, const GLRAttachmentType attachmentType, const uint32_t target)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->bindAttachment(attachment, attachmentType, target);
  }
  
  void fboResize(const ID fbo, const uint32_t width, const uint32_t height)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->resize(width, height);
  }
  
  void fboClear(const ID fbo)
  {
    if(!fbos.contains(fbo))
    {
      return;
    }
    fbos.at(fbo)->clear();
  }

  //Atlas
  void atlasUse(const ID atlas, const ID texture)
  {
    if(!atlases.contains(atlas) || !textures.contains(texture))
    {
      return;
    }
    atlases.at(atlas)->use(*textures.at(texture));
  }
  
  void atlasAddTile(const ID atlas, const std::string& name, const uint8_t channels, std::vector<uint8_t>&& tileData, const uint32_t width, const uint32_t height)
  {
    if(!atlases.contains(atlas))
    {
      return;
    }
    atlases.at(atlas)->addTile(name, tileData, channels, width, height);
  }
  
  QuadUVs atlasGetUVsForTile(const ID atlas, const std::string& name)
  {
    if(!atlases.contains(atlas))
    {
      return{};
    }
    return atlases.at(atlas)->getUVsForTile(name);
  }
  
  vec2<float> atlasGetTileDimensions(const ID atlas, const std::string& name)
  {
    if(!atlases.contains(atlas))
    {
      return{};
    }
    return atlases.at(atlas)->getTileDimensions(name);
  }
  
  void atlasFinalize(const ID atlas, const std::string& name, const ID texture, const uint8_t channels)
  {
    if(!atlases.contains(atlas))
    {
      return;
    }
    atlases.at(atlas)->finalize(name, *textures.at(texture), channels);
  }
  
  //Shader pipeline
  void shaderPipelineUse(const ID shaderPipeline)
  {
    if(!shaderPipelines.contains(shaderPipeline))
    {
      return;
    }
    shaderPipelines.at(shaderPipeline)->use();
  }
  
  void append(const ID shaderPipeline, const ID shader)
  {
    if(!shaderPipelines.contains(shaderPipeline) || !shaders.contains(shader))
    {
      return;
    }
    shaderPipelines.at(shaderPipeline)->append(shaders.at(shader).get());
  }
  
  void shaderPipelineSendUniforms(const ID shaderPipeline)
  {
    if(!shaderPipelines.contains(shaderPipeline))
    {
      return;
    }
    shaderPipelines.at(shaderPipeline)->sendUniforms();
  }
}
