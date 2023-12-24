#include "glrImage.hh"

namespace glr
{
  Image::Image(size_t width, size_t height)
  {
    this->imageData.resize(width * height);
  }
  
  Image::Image(size_t width, size_t height, Color const &color)
  {
    this->imageData.resize(width * height);
    FillOperation fillOperation = FillOperation(color);
    fillOperation.run(*this);
  }
  
  Image::Image(const uint32_t *data)
  {
    //TODO
  }
  
  Image::Image(Image &&moveFrom) noexcept
  {
    this->imageData = moveFrom.imageData;
    moveFrom.imageData = {};
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->bitDepth = moveFrom.bitDepth;
    moveFrom.bitDepth = 0;
  }
  
  Image& Image::operator=(Image &&moveFrom) noexcept
  {
    this->imageData = moveFrom.imageData;
    moveFrom.imageData = {};
    
    this->width = moveFrom.width;
    moveFrom.width = 0;
    
    this->height = moveFrom.height;
    moveFrom.height = 0;
    
    this->bitDepth = moveFrom.bitDepth;
    moveFrom.bitDepth = 0;
    
    return *this;
  }
  
  Color *Image::getImageData()
  {
    return this->imageData.data();
  }
  
  Color *Image::getRow(size_t row)
  {
    return this->imageData.data() + row * this->width;
  }
  
  size_t Image::index(size_t x, size_t y) const
  {
    return x + (y * this->width);
  }
  
  void Image::expand(size_t xAmount, size_t yAmount, Color const &newPixelsColor)
  {
    std::vector<Color> newData((this->width + xAmount) * (this->height + yAmount), newPixelsColor);
    for(size_t row = 0; row < this->height; row++)
    {
      Color *ndRow = newData.data() + row * this->width;
      memcpy(ndRow, this->getRow(row), this->width * sizeof(Color));
    }
    this->width += xAmount;
    this->height += yAmount;
    this->imageData = std::move(newData);
  }
  
  void Image::copyFrom(Image const &src, size_t xOffset, size_t yOffset)
  {
    std::vector<Color> newData;
    //TODO
    this->imageData = newData;
  }
  
  Color &Image::getPixel(size_t x, size_t y)
  {
    return this->imageData[this->index(x, y)];
  }
  
  ImageOperation::ImageOperation()
  {}
  ReplaceColorOperation::ReplaceColorOperation(Color const &src, Color const &dst) : src(src), dst(dst)
  {}
  FillOperation::FillOperation(Color const &fillColor) : fillColor(fillColor)
  {}
  
  ImageOperation::~ImageOperation() noexcept
  {}
  FillOperation::~FillOperation() noexcept
  {}
  ReplaceColorOperation::~ReplaceColorOperation() noexcept
  {}
  
  void FillOperation::run(Image &image)
  {
    for(auto &pix: image.imageData) pix = this->fillColor;
  }
  
  void ReplaceColorOperation::run(Image &image)
  {
    for(auto &pix: image.imageData) if(pix == this->src) pix = this->dst;
  }
}
