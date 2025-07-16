#pragma once

#include "export.hh"
#include "glrLogging.hh"
#include "glrAssetID.hh"
#include "glrEnums.hh"
#include "glrMesh.hh"
#include "glrTexture.hh"
#include "glrShader.hh"
#include "glrAtlas.hh"
#include "glrFramebuffer.hh"
#include "glrShaderPipeline.hh"

#include <string>

namespace glr::asset_repo
{
  //Management
  GLRENDER_API ID newShader(const std::string& shaderName, const std::string& vertSrc, const std::string& fragSrc);
  GLRENDER_API ID newShader(const std::string& shaderName, const std::string& compSrc);
  GLRENDER_API ID newTexture(const std::string& textureName, const uint8_t* data, uint32_t width, uint32_t height, uint8_t channels, GLRFilterMode min = GLRFilterMode::NEAREST, GLRFilterMode mag = GLRFilterMode::NEAREST, bool sRGB = false);
  GLRENDER_API ID newMesh();
  GLRENDER_API ID newFBO();
  GLRENDER_API ID newAtlas();
  GLRENDER_API ID newShaderPipeline();

  GLRENDER_API bool shaderExists(ID shader);
  GLRENDER_API bool textureExists(ID texture);
  GLRENDER_API bool meshExists(ID mesh);
  GLRENDER_API bool fboExists(ID fbo);
  GLRENDER_API bool atlasExists(ID atlas);
  GLRENDER_API bool shaderPipelineExists(ID shaderPipeline);

  GLRENDER_API void deleteShader(ID shader);
  GLRENDER_API void deleteTexture(ID texture);
  GLRENDER_API void deleteMesh(ID mesh);
  GLRENDER_API void deleteFBO(ID fbo);
  GLRENDER_API void deleteAtlas(ID atlas);
  GLRENDER_API void deleteShaderPipeline(ID shaderPipeline);

  GLRENDER_API void deleteShaders();
  GLRENDER_API void deleteTextures();
  GLRENDER_API void deleteMeshes();
  GLRENDER_API void deleteFBOs();
  GLRENDER_API void deleteAtlases();
  GLRENDER_API void deleteShaderPipelines();
  GLRENDER_API void deleteAll();

  //Method forwarding
  //Shaders
  GLRENDER_API void shaderUse(ID shader);
  GLRENDER_API void shaderSetUniform(ID shader, const std::string& name, const Shader::UniformValue& val);
  GLRENDER_API void shaderSendUniforms(ID shader);
  
  //Texture
  GLRENDER_API void textureUse(ID texture);
  GLRENDER_API void textureUseAsImage(ID texture, uint32_t target, GLRIOMode mode, GLRColorFormat format); //Bind a texture as an image for compute shaders, even if the texture wasn't set up to be used that way
  GLRENDER_API void textureSetBindingTarget(ID texture, uint32_t target);
  GLRENDER_API void textureSetFilterMode(ID texture, GLRFilterMode min, GLRFilterMode mag);
  GLRENDER_API void textureSetAnisotropyLevel(ID texture, uint32_t level);
  GLRENDER_API void textureSubImage(ID texture, const uint8_t* data, uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, uint8_t channels);
  GLRENDER_API void textureClear(ID texture);
  GLRENDER_API DownloadedImageData textureDownload(ID texture, uint8_t channels);
  GLRENDER_API uint32_t textureGetHandle(ID texture);
  
  //Mesh
  GLRENDER_API void meshUse(ID mesh);
  GLRENDER_API void meshSetPositionDimensions(ID mesh, GLRDimensions dimensions);
  GLRENDER_API void meshAddPositions(ID mesh, const float* positions, size_t positionsSize, const LoggingCallback& callback = nullptr);
  GLRENDER_API void meshAddIndices(ID mesh, const uint32_t* indices, size_t indicesSize, const LoggingCallback& callback = nullptr);
  GLRENDER_API void meshAddUVs(ID mesh, const float* uvs, size_t uvsSize, const LoggingCallback& callback = nullptr);
  GLRENDER_API void meshAddNormals(ID mesh, const float* normals, size_t normalsSize, const LoggingCallback& callback = nullptr);
  GLRENDER_API void meshAddColors(ID mesh, const float* colors, size_t colorsSize, const LoggingCallback& callback = nullptr);
  GLRENDER_API void meshFinalize(ID mesh, const LoggingCallback& callback = nullptr);
  GLRENDER_API size_t meshGetVertices(ID mesh);
  GLRENDER_API size_t meshGetIndices(ID mesh);
  GLRENDER_API bool meshIsFinalized(ID mesh);
  GLRENDER_API bool meshIsIndexed(ID mesh);
  GLRENDER_API GLRBufferType meshGetBufferType(ID mesh);
  GLRENDER_API GLRDrawMode meshGetDrawMode(ID mesh);
  GLRENDER_API GLRDrawType meshGetDrawType(ID mesh);

  //Framebuffer
  GLRENDER_API void fboUse(ID fbo);
  GLRENDER_API void fboSetDimensions(ID fbo, uint32_t width, uint32_t height);
  GLRENDER_API void fboAddColorAttachment(ID fbo, GLRAttachmentType attachmentType, uint8_t channels);
  GLRENDER_API void fboAddDepthAttachment(ID fbo, GLRAttachmentType attachmentType);
  GLRENDER_API void fboAddStencilAttachment(ID fbo, GLRAttachmentType attachmentType);
  GLRENDER_API void fboFinalize(ID fbo);
  GLRENDER_API void fboBindAttachment(ID fbo, GLRAttachment attachment, GLRAttachmentType attachmentType, uint32_t target);
  GLRENDER_API void fboResize(ID fbo, uint32_t width, uint32_t height);
  GLRENDER_API void fboClear(ID fbo);

  //Atlas
  GLRENDER_API void atlasUse(ID atlas, ID texture);
  GLRENDER_API void atlasAddTile(ID atlas, const std::string& name, uint8_t channels, std::vector<uint8_t>&& tileData, uint32_t width, uint32_t height);
  GLRENDER_API QuadUVs atlasGetUVsForTile(ID atlas, const std::string& name);
  GLRENDER_API vec2<float> atlasGetTileDimensions(ID atlas, const std::string& name);
  GLRENDER_API void atlasFinalize(ID atlas, const std::string& name, ID texture, uint8_t channels);
  
  //Shader Pipeline
  GLRENDER_API void shaderPipelineUse(ID shaderPipeline);
  GLRENDER_API void append(ID shaderPipeline, ID shader);
  GLRENDER_API void shaderPipelineSendUniforms(ID shaderPipeline);
  
}
