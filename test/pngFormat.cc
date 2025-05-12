#include "pngFormat.hh"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

bool checkChannels(const uint8_t channels)
{
  if(channels != 3 && channels != 4)
  {
    return false;
  }
  return true;
}

PNG decodePNG(const std::vector<uint8_t>& file)
{
  PNG png{};
  int32_t width, height, channels;
  
  uint8_t* imageData = stbi_load_from_memory(file.data(), (int32_t)file.size(), &width, &height, &channels, 0);
  if(!imageData)
  {
    return {};
  }
  
  png.data.insert(png.data.begin(), imageData, imageData + (width * height * channels));
  stbi_image_free(imageData);
  
  png.width = width;
  png.height = height;
  png.bitDepth = 8;
  
  if(!checkChannels(channels))
  {
    return {};
  }
  
  png.channels = channels;
  return png;
}

PNG decodePNG(const std::string& filePath)
{
  PNG png{};
  int32_t width, height, channels;
  
  uint8_t* imageData = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
  if(!imageData)
  {
    return {};
  }
  
  png.data.insert(png.data.begin(), imageData, imageData + (width * height * channels));
  stbi_image_free(imageData);
  
  png.width = width;
  png.height = height;
  png.bitDepth = 8;
  
  if(!checkChannels(channels))
  {
    return {};
  }
  
  png.channels = channels;
  return png;
}

void writePNG(const std::string& filePath, const int32_t width, const int32_t height, const uint8_t* imageData, const uint8_t channels)
{
  if(!checkChannels(channels))
  {
    return;
  }
  stbi_write_png(filePath.c_str(), width, height, channels, imageData, width * channels);
}
