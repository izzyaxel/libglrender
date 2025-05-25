#pragma once

#include <cstdint>
#include <string>
#include <vector>

constexpr inline uint8_t CHANNELS_GREY = 1;
constexpr inline uint8_t CHANNELS_RGB = 3;
constexpr inline uint8_t CHANNELS_RGBA = 4;

struct PNG
{
  uint32_t width = 0;
  uint32_t height = 0;
  uint8_t channels = 0;
  uint8_t bitDepth = 0;

  std::vector<uint8_t> data{};
};

PNG decodePNG(const std::vector<uint8_t>& file);
PNG decodePNG(const std::string& filePath);
void writePNG(const std::string& filePath, int32_t width, int32_t height, const uint8_t* imageData, uint8_t channels);
