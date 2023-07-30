#pragma once

namespace glr
{
  #if defined(WINDOWS)
#define GLRENDER_API __declspec(dllexport)
  #else
#define GLRender_API
  #endif
}

