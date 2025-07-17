#include "png/pngFormat.hh"
#include "deltatimer.hh"

#include <glrender/glrAssetRepository.hh>
#include <glrender/glrFixedRenderer.hh>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <string>
#include <filesystem>

constexpr int32_t width = 800;
constexpr int32_t height = 600;

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

const std::vector<uint32_t> quadIndices{0, 1, 2, 2, 3, 0};
const std::vector quadPositionsIndexed{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  -0.5f, 0.5f};
const std::vector quadUVsIndexed{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f};

/*std::vector quadPositions{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f};
std::vector quadUVs{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f};*/

constexpr std::array fullscreenQuadVerts{-1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, 1.0f,  1.0f, 1.0f,};
constexpr std::array fullscreenQuadUVs{0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f};

const std::string testFrag =
R"(#version 460 core

in vec2 uv;
out vec4 fragColor;

void main()
{
fragColor = vec4(1.0);
})";

const std::string objectFrag =
R"(#version 460 core

in vec2 uv;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D tex;

void main()
{
fragColor = texture(tex, uv);
})";

const std::string commonVert =
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

const std::string comp =
R"(#version 460 core

layout(local_size_x = 40, local_size_y = 20) in;
layout(rgba32f, binding = 0) uniform image2D imageOut;

void main()
{
ivec2 current = ivec2(gl_GlobalInvocationID.xy);
imageStore(imageOut, current, vec4(1.0));
})";

const std::string transferFrag =
R"(#version 460 core

in vec2 uv;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
fragColor = texture(tex, uv);
})";

PNG png;

glr::ID testShader = glr::INVALID_ID;
glr::ID objectShader = glr::INVALID_ID;
glr::ID transferShader = glr::INVALID_ID;
glr::ID objectTexture = glr::INVALID_ID;
glr::ID fbo = glr::INVALID_ID;
glr::ID objectMesh = glr::INVALID_ID;
glr::ID fullscreenMesh = glr::INVALID_ID;
glr::ID pipelineID = glr::INVALID_ID;

glr::RenderList renderList{};
glr::Renderable renderableA;
glr::Renderable renderableB;

std::unique_ptr<glr::Renderer> renderer = nullptr;

bool exiting = false;

void setup()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow("Fixed Function Renderer Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
  if(!window)
  {
    throw std::runtime_error("Failed to create a window");
  }

  SDL_GL_LoadLibrary(nullptr);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  context = SDL_GL_CreateContext(window);
  if(!context)
  {
    throw std::runtime_error("Failed to create OpenGL context");
  }
  SDL_GL_SetSwapInterval(1);
  
  renderer = std::make_unique<glr::Renderer>(reinterpret_cast<glr::GLLoadFunc>(SDL_GL_GetProcAddress), width, height);
}

struct Camera
{
  quat<float> rotation{};
  vec3<float> position{0.0f, 0.0f, 1.0f};
  float fov = 45.0f;
  float near = 0.01f;
  float far = 100.0f;

  mat4x4<float> view = viewMatrix(rotation, position);
  mat4x4<float> orthoProjection = orthoProjectionMatrix(width / -2.0f, width / 2.0f, height / 2.0f, height / -2.0f, near, far);
  mat4x4<float> perspectiveProjection = perspectiveProjectionMatrix(fov, near, far, width, height);
} inline camera;

void initAssets()
{
  png = decodePNG(std::filesystem::current_path().string() + "/test.png");

  testShader = glr::asset_repo::newShader("test", commonVert, testFrag);
  objectShader = glr::asset_repo::newShader("object", commonVert, objectFrag);
  transferShader = glr::asset_repo::newShader("transfer", commonVert, transferFrag);
  objectTexture = glr::asset_repo::newTexture("texture", png.data.data(), png.width, png.height, png.channels);
  
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

  #if 1
  renderableA = glr::newRenderable({glr::OBJECT_RENDERABLE_TEMPLATE});
  renderableA.fragVertShaderComp->shader = glr::asset_repo::newShader("default", commonVert, objectFrag);
  renderableA.textureComp->texture = glr::asset_repo::newTexture("test texture", png.data.data(), png.width, png.height, png.channels);
  renderableA.meshComp->mesh = glr::asset_repo::newMesh();
  glr::asset_repo::meshSetPositionDimensions(renderableA.meshComp->mesh, GLRDimensions::TWO_DIMENSIONAL);
  glr::asset_repo::meshAddPositions(renderableA.meshComp->mesh, quadPositionsIndexed.data(), quadPositionsIndexed.size());
  glr::asset_repo::meshAddUVs(renderableA.meshComp->mesh, quadUVsIndexed.data(), quadUVsIndexed.size());
  glr::asset_repo::meshAddIndices(renderableA.meshComp->mesh, quadIndices.data(), quadIndices.size());
  glr::asset_repo::meshFinalize(renderableA.meshComp->mesh);
  renderableA.transformComp->pos =  vec3{0.0f, 0.0f, 0.0f};
  renderableA.transformComp->scale = vec3{400.0f, 400.0f, 1.0f};
  renderList.add(renderableA);
  #else
  //TODO FIXME compute shader isn't running, the framebuffers are interfering?
  renderableB = glr::newRenderable({glr::COMPUTE_RENDERABLE_TEMPLATE});
  renderableA.computeShaderComp->shader = std::make_shared<glr::Shader>("default", comp);
  renderList.add(renderableB);
  #endif
}

int main()
{
  setup();
  initAssets();
  
  DeltaTimer dt(60);
  while(!exiting)
  {
    dt.update();
    if(dt.isTargetReached())
    {
      dt.onTargetReached();
      SDL_Event event;
      while(SDL_PollEvent(&event) != 0)
      {
        switch(event.type)
        {
          case SDL_QUIT:
          {
            exiting = true;
            break;
          }
          default: break;
        }
      }
      
      renderer->render(renderList, camera.view, camera.orthoProjection);
      SDL_GL_SwapWindow(window);
      
      if(dt.frames % (uint64_t)dt.getFPS() == 0)
      {
        printf("%.4f ms (%.1f FPS)  \r", dt.getDeltaTime() * 1000.0f, 1.0f / dt.getDeltaTime());
      }
      dt.frames++;
    }
  }
  
  renderer.reset();
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
