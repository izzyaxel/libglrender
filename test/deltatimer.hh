#pragma once

#include <chrono>

struct DeltaTimer
{
  explicit DeltaTimer(const uint32_t targetFPS);
  
  void setTargetFPS(const uint32_t targetFPS);
  void update();
  bool isTargetReached() const;
  void onTargetReached();
  uint32_t getFPS() const;
  float getDeltaTime() const;

  uint64_t frames = 0;
  
  private:
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point prevLoop = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point prevFrame = std::chrono::steady_clock::now();
  float accumulator = 0.0f;
  float deltaTime = 0.0f;
  float fps = 60.0f;
  float target = 1.0f / 60.0f;
};
