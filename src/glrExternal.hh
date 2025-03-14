#pragma once

#include <cstdint>
#include <vector>

#include "export.hh"

namespace glr
{
  GLRENDER_API void pixelStoreiPack(int i);
  GLRENDER_API void pixelStoreiUnpack(int i);
  GLRENDER_API std::vector<uint8_t> getPixels(uint32_t width, uint32_t height);
}
