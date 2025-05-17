#include "pngFormat.hh"

#include <glrender/glrMesh.hh>
#include <glrender/glrShader.hh>
#include <glrender/glrRenderer.hh>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <string>
#include <chrono>
#include <filesystem>

constexpr int32_t width = 800;
constexpr int32_t height = 600;

inline std::vector<uint32_t> quadIndices{0, 1, 2, 2, 3, 0};
inline std::vector quadPositionsIndexed{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  -0.5f, 0.5f};
inline std::vector quadUVsIndexed{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f};

inline std::vector quadPositions{-0.5f, -0.5f,  0.5f, -0.5f,  0.5f, 0.5f,  0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f};
inline std::vector quadUVs{0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f};

std::string frag = R"(#version 460 core

in vec2 uv;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D tex;

void main()
{
  fragColor = texture(tex, uv);
})";

std::string vert = R"(#version 460 core

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;

out vec2 uv;

uniform mat4 mvp;

void main()
{
  uv = uv_in;
  gl_Position = mvp * vec4(pos_in, 1.0);
})";

std::string comp = R"(#version 460 core

layout(local_size_x = 40, local_size_y = 20) in;
layout(rgba32f, binding = 0) uniform image2D imageOut;

void main()
{
  ivec2 current = ivec2(gl_GlobalInvocationID.xy);
  imageStore(imageOut, current, vec4(1.0));
})";

bool exiting = false;

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;
std::unique_ptr<glr::Renderer> renderer = nullptr;

void eventPump()
{
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
}

void setup()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow("gltest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
  if(!window)
  {
    throw std::runtime_error("Failed to create a window");
  }

  SDL_GL_LoadLibrary(nullptr);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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

void cleanup()
{
  renderer.reset();
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
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
};

int main()
{
  setup();
  const Camera camera{};
  glr::RenderList renderList;

  #if 1
  PNG png = decodePNG(std::filesystem::current_path().string() + "/test.png");
  const glr::Renderable renderable = glr::newRenderable({glr::OBJECT_RENDERABLE_TEMPLATE});
  renderable.fragVertShaderComp->shader = std::make_shared<glr::Shader>("default", vert, frag);
  renderable.textureComp->texture = std::make_shared<glr::Texture>("test texture", png.data.data(), png.width, png.height, png.channels);
  renderable.meshComp->mesh = std::make_shared<glr::Mesh>();
  renderable.meshComp->mesh->setPositionDimensions(GLRDimensions::TWO_DIMENSIONAL);
  renderable.meshComp->mesh->addPositions(quadPositionsIndexed.data(), quadPositionsIndexed.size())->addUVs(quadUVsIndexed.data(), quadUVsIndexed.size())->addIndices(quadIndices.data(), quadIndices.size())->finalize();
  renderable.transformComp->pos =  vec3{0.0f, 0.0f, 0.0f};
  renderable.transformComp->scale = vec3{400.0f, 400.0f, 1.0f};
  #else //TODO FIXME compute shader isn't running, the framebuffers are interfering, or texture bindings arent properly configured
  const glr::Renderable renderable = glr::newRenderable({glr::COMPUTE_RENDERABLE_TEMPLATE});
  renderable.computeShaderComp->shader = std::make_shared<glr::Shader>("default", comp);
  #endif
  
  renderList.add(renderable);
  
  auto prevLoop = std::chrono::steady_clock::now();
  auto prevFrame = std::chrono::steady_clock::now();
  float accumulator = 0.0f;
  float deltaTime = 0.0f;
  constexpr float fps = 60.0f;
  constexpr float target = 1.0f / fps;
  uint64_t frames = 0;
  while(!exiting)
  {
    auto now = std::chrono::steady_clock::now();
    accumulator += std::chrono::duration_cast<std::chrono::duration<float>>(now - prevLoop).count();
    prevLoop = now;
    if(accumulator >= target)
    {
      deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - prevFrame).count();
      accumulator -= target - 0.00001f;
      prevFrame = now;
     
      eventPump();
      renderer->render(renderList, camera.view, camera.orthoProjection);
      SDL_GL_SwapWindow(window);
      
      if(frames % (uint64_t)fps == 0)
      {
        printf("%.4f ms (%.1f FPS)  \r", deltaTime * 1000.0f, 1.0f / deltaTime);
      }
      frames++;
    }
  }
  
  cleanup();
  return 0;
}
