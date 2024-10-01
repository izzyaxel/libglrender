#pragma once

#include "glrColor.hh"

#include <cstddef>
#include <vector>

namespace glr
{
  struct Image
  {
    /// Blank image constructor
    GLRENDER_API Image(size_t width, size_t height);
    
    /// Single color constructor
    GLRENDER_API Image(size_t width, size_t height, const Color& color);
    
    /// Image data constructor
    GLRENDER_API explicit Image(const uint32_t* data);
    
    Image(Image const &copyFrom) = delete;
    Image& operator=(const Image& copyFrom) = delete;
    GLRENDER_API Image(Image&& moveFrom) noexcept;
    GLRENDER_API Image& operator=(Image&& moveFrom) noexcept;
    
    [[nodiscard]] GLRENDER_API Color* getImageData();
    
    [[nodiscard]] GLRENDER_API Color* getRow(size_t row);
    
    /// Get the pixel at the given x, y position in the image, starting from the top-left, left to right, top to bottom
    [[nodiscard]] GLRENDER_API Color& getPixel(size_t x, size_t y);
    
    /// Get the index into the imageData array based on the x, y position of the pixel given
    [[nodiscard]] GLRENDER_API size_t index(size_t x, size_t y) const;
    
    GLRENDER_API void expand(size_t xAmount, size_t yAmount, const Color& newPixelsColor);
    
    GLRENDER_API void copyFrom(const Image& src, size_t xOffset, size_t yOffset);
    
    std::vector<Color> imageData{};
    size_t width = 0;
    size_t height = 0;
    char bitDepth = 8;
  };
  
  struct ImageOperation
  {
    GLRENDER_API virtual ~ImageOperation() = 0;
    GLRENDER_API virtual void run(Image& image) = 0;
    
    protected:
    GLRENDER_API ImageOperation();
  };
  
  struct FillOperation final : ImageOperation
  {
    GLRENDER_API ~FillOperation() override;
    GLRENDER_API explicit FillOperation(const Color& fillColor);
    GLRENDER_API void run(Image& image) override;
    
    Color fillColor{};
  };
  
  struct ReplaceColorOperation final : ImageOperation
  {
    GLRENDER_API ~ReplaceColorOperation() override;
    GLRENDER_API ReplaceColorOperation(const Color& src, const Color& dst);
    GLRENDER_API void run(Image& image) override;
    
    Color src{};
    Color dst{};
  };
}
