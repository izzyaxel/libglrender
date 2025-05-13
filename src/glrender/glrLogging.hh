#pragma once

#include "glrEnums.hh"

#include <functional>
#include <string>

namespace glr
{
  using LoggingCallback = std::function<void(GLRLogType, const std::string&)>;
}
