#pragma once

#include <functional>
#include <string>

namespace glr
{
  enum struct LogType
  {
    INFO, WARNING, ERROR,
  };
  
  using LoggingCallback = std::function<void(LogType, const std::string&)>;
}
