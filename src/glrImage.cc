#include "glrender/glrImage.hh"

namespace glr
{
  Image::Image(const size_t width, const size_t height)
  {
    this->imageData.resize(width * height);
  }
  
  Image::Image(const size_t width, const size_t height, const Color& color)
  {
    this->imageData.resize(width * height);
    FillOperation fillOperation = FillOperation(color);
    fillOperation.run(*this);
  }
  
  Image::Image(const uint32_t* data)
  {
    //TODO
  }
  
  Image::Image(Image&& moveFrom) noexcept
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
  
  Image& Image::operator=(Image&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
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
  
  Color* Image::getImageData()
  {
    return this->imageData.data();
  }
  
  Color* Image::getRow(const size_t row)
  {
    return this->imageData.data() + row * this->width;
  }
  
  size_t Image::index(const size_t x, const size_t y) const
  {
    return x + (y * this->width);
  }
  
  void Image::expand(const size_t xAmount, const size_t yAmount, const Color& newPixelsColor)
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
  
  void Image::copyFrom(const Image& src, const size_t xOffset, const size_t yOffset)
  {
    std::vector<Color> newData;
    //TODO
    this->imageData = newData;
  }
  
  Color &Image::getPixel(const size_t x, const size_t y)
  {
    return this->imageData[this->index(x, y)];
  }
  
  ImageOperation::ImageOperation()
  {}
  ReplaceColorOperation::ReplaceColorOperation(const Color& src, const Color& dst) :
  src(src), dst(dst)
  {}
  FillOperation::FillOperation(const Color& fillColor) :
  fillColor(fillColor)
  {}
  
  ImageOperation::~ImageOperation() noexcept
  {}
  FillOperation::~FillOperation() noexcept
  {}
  ReplaceColorOperation::~ReplaceColorOperation() noexcept
  {}
  
  void FillOperation::run(Image& image)
  {
    for(auto &pix: image.imageData) pix = this->fillColor;
  }
  
  void ReplaceColorOperation::run(Image& image)
  {
    for(auto &pix: image.imageData) if(pix == this->src) pix = this->dst;
  }
}
