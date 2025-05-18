#pragma once

#include <cstdint>
#include <limits>

namespace glr
{
  using ID = uint64_t;
  inline constexpr ID INVALID_ID = std::numeric_limits<ID>::max();
}
