#pragma once

#include "glrEnums.hh"

#include <functional>
#include <string>

namespace glr
{
  using LoggingCallback = std::function<void(LogType, const std::string&)>;
}
