#include "glrImage.hh"

namespace GLRender
{
	Image::Image(size_t width, size_t height)
	{
		this->m_imageData.resize(width * height);
	}
	
	Image::Image(size_t width, size_t height, Color const &color)
	{
		this->m_imageData.resize(width * height);
		FillOperation fillOperation = FillOperation(color);
		fillOperation.run(*this);
	}
	
	Image::Image(const uint32_t *data)
	{
		//TODO
	}
	
	Color* Image::getImageData()
	{
		return this->m_imageData.data();
	}
	
	Color* Image::getRow(size_t row)
	{
		return this->m_imageData.data() + row * this->m_width;
	}
	
	size_t Image::index(size_t x, size_t y) const
	{
		return x + (y * this->m_width);
	}
	
	void Image::expand(size_t xAmount, size_t yAmount, Color const &newPixelsColor)
	{
		std::vector<Color> newData((this->m_width + xAmount) * (this->m_height + yAmount), newPixelsColor);
		for(size_t row = 0; row < this->m_height; row++)
		{
			Color *ndRow = newData.data() + row * this->m_width;
			memcpy(ndRow, this->getRow(row), this->m_width * sizeof(Color));
		}
		this->m_width += xAmount;
		this->m_height += yAmount;
		this->m_imageData = std::move(newData);
	}
	
	void Image::copyFrom(Image const &src, size_t xOffset, size_t yOffset)
	{
		std::vector<Color> newData;
		//TODO
		this->m_imageData = newData;
	}
	
	Color& Image::getPixel(size_t x, size_t y)
	{
		return this->m_imageData[this->index(x, y)];
	}
	
	void FillOperation::run(Image &image)
	{
		for(auto &pix : image.m_imageData) pix = this->m_fillColor;
	}
	
	void ReplaceColorOperation::run(Image &image)
	{
		for(auto &pix : image.m_imageData) if(pix == this->m_src) pix = this->m_dst;
	}
}
