#pragma once

namespace glr
{
  #if defined(WINDOWS)
  #define GLRENDER_API __declspec(dllexport)
  #else
  #define GLRENDER_API
  #endif
}

