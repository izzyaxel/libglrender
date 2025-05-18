#pragma once
#include "glrender/glrAssetRepository.hh"
#include "glrender/glrEnums.hh"

#include <array>
#include <vector>
#include <filesystem>

namespace Assets
{
  inline const std::vector<uint32_t> quadIndices{0, 1, 2, 2, 3, 0};
  inline const std::vector quadPositionsIndexed{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  -0.5f, 0.5f};
  inline const std::vector quadUVsIndexed{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f};

  /*std::vector quadPositions{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f};
  std::vector quadUVs{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f};*/
  
  inline constexpr std::array fullscreenQuadVerts{-1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,};
  inline constexpr std::array fullscreenQuadUVs{0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f};

  inline const std::string objectFrag =
R"(#version 460 core

in vec2 uv;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D tex;

void main()
{
  fragColor = texture(tex, uv);
})";

  inline const std::string commonVert =
R"(#version 460 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;

out vec2 uv;

uniform mat4 mvp;

void main()
{
  uv = uv_in;
  gl_Position = mvp * vec4(pos_in, 1.0);
})";

  /*std::string comp =
*R"(#version 460 core
  
layout(local_size_x = 40, local_size_y = 20) in;
layout(rgba32f, binding = 0) uniform image2D imageOut;
  
void main()
{
  ivec2 current = ivec2(gl_GlobalInvocationID.xy);
  imageStore(imageOut, current, vec4(1.0));
})";*/

  inline const std::string transferFrag =
R"(#version 460 core

in vec2 uv;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
  fragColor = texture(tex, uv);
})";
  
  inline glr::ID objectShader = glr::INVALID_ID;
  inline glr::ID transferShader = glr::INVALID_ID;
  inline glr::ID objectTexture = glr::INVALID_ID;
  inline glr::ID fbo = glr::INVALID_ID;
  inline glr::ID objectMesh = glr::INVALID_ID;
  inline glr::ID fullscreenMesh = glr::INVALID_ID;
  inline glr::ID pipelineID = glr::INVALID_ID;

  inline void init()
  {
    const PNG png = decodePNG(std::filesystem::current_path().string() + "/test.png");
    
    objectShader = glr::asset_repo::newShader("pipeline object", commonVert, objectFrag);
    transferShader = glr::asset_repo::newShader("pipeline transfer", commonVert, transferFrag);
    objectTexture = glr::asset_repo::newTexture("pipeline texture", png.data.data(), png.width, png.height, png.channels);
  
    fbo = glr::asset_repo::newFBO();
    glr::asset_repo::fboAddColorAttachment(fbo, GLRAttachmentType::TEXTURE, 4);
  
    objectMesh = glr::asset_repo::newMesh();
    glr::asset_repo::meshSetPositionDimensions(objectMesh, GLRDimensions::TWO_DIMENSIONAL);
    glr::asset_repo::meshAddIndices(objectMesh, quadIndices.data(), quadIndices.size());
    glr::asset_repo::meshAddPositions(objectMesh, quadPositionsIndexed.data(), quadPositionsIndexed.size());
    glr::asset_repo::meshAddUVs(objectMesh, quadUVsIndexed.data(), quadUVsIndexed.size());
    glr::asset_repo::meshFinalize(objectMesh);

    fullscreenMesh = glr::asset_repo::newMesh();
    glr::asset_repo::meshSetPositionDimensions(fullscreenMesh, GLRDimensions::TWO_DIMENSIONAL);
    glr::asset_repo::meshAddPositions(fullscreenMesh, fullscreenQuadVerts.data(), fullscreenQuadVerts.size());
    glr::asset_repo::meshAddUVs(fullscreenMesh, fullscreenQuadUVs.data(), fullscreenQuadUVs.size());
    glr::asset_repo::meshFinalize(fullscreenMesh);
  }
}
