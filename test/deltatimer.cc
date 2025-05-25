#include "deltatimer.hh"

DeltaTimer::DeltaTimer(const uint32_t targetFPS)
{
  this->fps = (float)targetFPS;
  this->target = 1.0f / this->fps;
}

void DeltaTimer::update()
{
  this->now = std::chrono::steady_clock::now();
  this->accumulator += std::chrono::duration_cast<std::chrono::duration<float>>(this->now - this->prevLoop).count();
  this->prevLoop = this->now;
}

bool DeltaTimer::isTargetReached() const
{
  return this->accumulator >= this->target;
}

void DeltaTimer::onTargetReached()
{
  this->deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - this->prevFrame).count();
  this->accumulator -= this->target - 0.00001f;
  this->prevFrame = this->now;
}

uint32_t DeltaTimer::getFPS() const
{
  return (uint32_t)this->fps;
}

float DeltaTimer::getDeltaTime() const
{
  return this->deltaTime;
}
