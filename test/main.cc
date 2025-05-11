#include <glrender/glrMesh.hh>
#include <glrender/glrShader.hh>
#include <glrender/glrRenderer.hh>
#define SDL_MAIN_HANDLED
#include <chrono>
#include <SDL2/SDL.h>
#include <string>

constexpr int32_t width = 800;
constexpr int32_t height = 600;

inline std::vector triUVs{0.f, 0.f,   1.f, 0.f,   1.f, 1.f};
inline std::vector triPositions{-0.5f, -0.5f, 0.0f,   0.5f, -0.5f, 0.0f,   0.0f, 0.5f, 0.0f};

inline std::vector quadIndices{0, 1, 2, 2, 3, 1};
inline std::vector quadUVs{0.f, 0.f,   1.f, 0.f,   1.f, 1.f,   1.f, 1.f,   0.f, 1.f,   0.f, 0.f};
inline std::vector quadPositions{-0.5f, -0.5f, 0.f,   0.5f, -0.5f, 0.f,   0.5f, 0.5f, 0.f,   0.5f, 0.5f, 0.f,   -0.5f, 0.5f, 0.f,   -0.5f, -0.5f, 0.f};

std::string frag = R"(#version 450

in vec2 uv;
in vec4 pos;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

vec3 rainbow(float level)
{
  float r = float(level <= 2.0) + float(level > 4.0) * 0.5;
  float g = max(1.0 - abs(level - 2.0) * 0.5, 0.0);
  float b = (1.0 - (level - 4.0) * 0.5) * float(level >= 4.0);
  return vec3(r, g, b);
}

vec3 smoothRainbow (float x)
{
  float level1 = floor(x * 6.0);
  float level2 = min(6.0, floor(x * 6.0) + 1.0);
  
  vec3 a = rainbow(level1);
  vec3 b = rainbow(level2);
  
  return mix(a, b, fract(x * 6.0));
}

void main()
{
  fragColor = vec4(rainbow(floor(uv.x * 6.0)), 1.0);
})";

std::string vert = R"(#version 450

layout(location = 0) in vec3 pos_in;
layout(location = 1) in vec2 uv_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 color_in;
out vec2 uv;
out vec4 pos;
uniform mat4 mvp;

void main()
{
  uv = uv_in;
  pos = mvp * vec4(pos_in, 0);
  gl_Position = mvp * vec4(pos_in, 1.0);
})";

bool exiting = false;

SDL_Event event;
SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;
glr::Renderer* renderer = nullptr;

void eventPump()
{
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
  
  renderer = new glr::Renderer(reinterpret_cast<glr::GLLoadFunc>(SDL_GL_GetProcAddress), width, height);
}

void cleanup()
{
  delete renderer;
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

struct Camera
{
  quat<float> rotation{};
  vec3<float> position{0.0f, 0.0f, -100.0f};

  mat4x4<float> view = viewMatrix(rotation, position);
  //mat4x4<float> projection = orthoProjectionMatrix(width / -2.0f, width / 2.0f, height / 2.0f, height / -2.0f, 0.01f, 1000.0f);
  mat4x4<float> projection = perspectiveProjectionMatrix(45.0f, 0.01f, 1000.0f, width, height);
};

int main()
{
  setup();

  float angle = 0.0f;
  Camera camera{};
  camera.view = viewMatrix(camera.rotation, camera.position);
  
  const glr::Renderable renderable = glr::newRenderable({glr::OBJECT_RENDERABLE_TEMPLATE});
  renderable.fragVertShaderComp->shader = std::make_shared<glr::Shader>("default", vert, frag);
  renderable.meshComp->mesh = std::make_shared<glr::Mesh>();
  renderable.meshComp->mesh->bufferType = GLBufferType::SEPARATE;
  //renderable.meshComp->mesh->addPositions(triPositions.data(), triPositions.size())->addUVs(triUVs.data(), triUVs.size())->finalize();
  renderable.meshComp->mesh->addPositions(quadPositions.data(), quadPositions.size())->addUVs(quadUVs.data(), quadUVs.size())->finalize();
  renderable.transformComp->pos =  vec3{0.0f, 0.0f, -0.5f};
  renderable.transformComp->scale = vec3{80.0f, 50.0f, 1.0f};
  glr::RenderList renderList;
  renderList.add(renderable);
  renderer->setCullFace(false);

  auto prevLoop = std::chrono::steady_clock::now();
  auto prevFrame = std::chrono::steady_clock::now();
  float accumulator = 0.0f;
  float deltaTime = 0.0f;
  constexpr float target = 1.0f / 60.0f;
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

      angle += 1.f;
      renderable.transformComp->rotation.fromAxial(rand() % 2, rand() % 2, rand() % 2, angle); //The gays are aggressive
     
      eventPump();
      renderer->clearCurrentFramebuffer();
      renderer->render(renderList, camera.view, camera.projection);
      SDL_GL_SwapWindow(window);
      printf("%.4f ms (%.1f FPS)  \r", deltaTime * 1000.0f, 1.0f / deltaTime);
    }
  }
  
  cleanup();
  return 0;
}
