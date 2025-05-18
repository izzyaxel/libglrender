#include "pngFormat.hh"
#include "assets.hh"

#include <glrender/glrAssetRepository.hh>
#include <glrender/glrRenderer.hh>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <string>
#include <chrono>
#include <filesystem>

#define PIPELINE_RENDERING

constexpr int32_t width = 800;
constexpr int32_t height = 600;

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

#if defined(PIPELINE_RENDERING)
std::unique_ptr<glr::PipelineRenderer> pipelineRenderer = nullptr;
std::unique_ptr<glr::Pipeline> pipeline = nullptr;
#else
std::unique_ptr<glr::Renderer> renderer = nullptr;
#endif

bool exiting = false;

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
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  context = SDL_GL_CreateContext(window);
  if(!context)
  {
    throw std::runtime_error("Failed to create OpenGL context");
  }
  SDL_GL_SetSwapInterval(1);

  #if defined(PIPELINE_RENDERING)
  pipelineRenderer = std::make_unique<glr::PipelineRenderer>(reinterpret_cast<glr::GLLoadFunc>(SDL_GL_GetProcAddress), width, height);
  #else
  renderer = std::make_unique<glr::Renderer>(reinterpret_cast<glr::GLLoadFunc>(SDL_GL_GetProcAddress), width, height);
  #endif
}

void cleanup()
{
  #if defined(PIPELINE_RENDERING)
  pipelineRenderer.reset();
  #else
  renderer.reset();
  #endif
  
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
} inline camera;

#if defined(PIPELINE_RENDERING)
void setupPipeline()
{
  pipeline = std::make_unique<glr::Pipeline>();
  
  pipelineRenderer->model = modelMatrix({0.0f, 0.0f, 0.0f}, quat<float>{}, {400.0f, 400.0f, 1.0f});
  pipelineRenderer->view = camera.view;
  pipelineRenderer->projection = camera.orthoProjection;
  
  //Record commands into the pipeline in the order they should be played back
  /*pipeline->bindBackbuffer();
  pipeline->clearCurrentFramebuffer(GLRClearType::COLOR, GLRClearType::DEPTH);
  pipeline->bindTexture(Assets::objectTexture, 0);
  pipeline->bindShader(Assets::objectShader);
  pipeline->bindMesh(Assets::objectMesh);
  pipeline->calculateMVP();
  pipeline->setUniformMVP(Assets::objectShader);
  pipeline->sendUniforms(Assets::objectShader);
  pipeline->drawIndexed(GLRDrawMode::TRIS, glr::asset_repo::meshGetIndices(Assets::objectMesh), GLRIndexBufferType::UINT);*/

  pipeline->bindFramebuffer(Assets::fbo);
  pipeline->clearCurrentFramebuffer(GLRClearType::COLOR, GLRClearType::DEPTH);
  pipeline->bindTexture(Assets::objectTexture, 0);
  pipeline->bindShader(Assets::objectShader);
  pipeline->bindMesh(Assets::objectMesh);
  pipeline->calculateMVP();
  pipeline->setUniformMVP(Assets::objectShader);
  pipeline->sendUniforms(Assets::objectShader);
  pipeline->drawIndexed(GLRDrawMode::TRIS, glr::asset_repo::meshGetIndices(Assets::objectMesh), GLRIndexBufferType::UINT);

  pipeline->bindBackbuffer();
  pipeline->clearCurrentFramebuffer(GLRClearType::COLOR, GLRClearType::DEPTH);
  pipeline->bindMesh(Assets::fullscreenMesh);
  pipeline->bindShader(Assets::transferShader);
  pipeline->bindFramebufferAttachment(Assets::fbo, 0, GLRAttachment::COLOR, GLRAttachmentType::TEXTURE);
  pipeline->setUniformMVP(Assets::transferShader);
  pipeline->sendUniforms(Assets::transferShader);
  pipeline->draw(GLRDrawMode::TRI_STRIPS, glr::asset_repo::meshGetVertices(Assets::fullscreenMesh));

  Assets::pipelineID = pipelineRenderer->addPipeline(*pipeline);
  pipelineRenderer->usePipeline(Assets::pipelineID);
}
#endif

int main()
{
  setup();

  #if defined(PIPELINE_RENDERING)
  
  Assets::init();
  setupPipeline();
  
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
      pipelineRenderer->render();
      SDL_GL_SwapWindow(window);
      
      /*if(frames % (uint64_t)fps == 0)
      {
        printf("%.4f ms (%.1f FPS)  \r", deltaTime * 1000.0f, 1.0f / deltaTime);
      }*/
      frames++;
    }
  }
  #else
  glr::RenderList renderList;
  const glr::Renderable renderable = glr::newRenderable({glr::OBJECT_RENDERABLE_TEMPLATE});
  renderable.fragVertShaderComp->shader = std::make_shared<glr::Shader>("default", vert, frag);
  renderable.textureComp->texture = std::make_shared<glr::Texture>("test texture", png.data.data(), png.width, png.height, png.channels);
  renderable.meshComp->mesh = std::make_shared<glr::Mesh>();
  renderable.meshComp->mesh->setPositionDimensions(GLRDimensions::TWO_DIMENSIONAL);
  renderable.meshComp->mesh->addPositions(quadPositionsIndexed.data(), quadPositionsIndexed.size())->addUVs(quadUVsIndexed.data(), quadUVsIndexed.size())->addIndices(quadIndices.data(), quadIndices.size())->finalize();
  renderable.transformComp->pos =  vec3{0.0f, 0.0f, 0.0f};
  renderable.transformComp->scale = vec3{400.0f, 400.0f, 1.0f};
  
  //TODO FIXME compute shader isn't running, the framebuffers are interfering, or texture bindings arent properly configured
  /*const glr::Renderable renderable = glr::newRenderable({glr::COMPUTE_RENDERABLE_TEMPLATE});
  renderable.computeShaderComp->shader = std::make_shared<glr::Shader>("default", comp);
  renderList.add(renderable);*/
  
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
  #endif
  
  cleanup();
  return 0;
}
