#pragma once

#include "glrColor.hh"

#include <cstddef>
#include <vector>

namespace GLRender
{
	struct Image
	{
		/// Blank image constructor
		GLRENDER_API Image(size_t width, size_t height);
		
		/// Single color constructor
		GLRENDER_API Image(size_t width, size_t height, Color const &color);
		
		/// Image data constructor
		GLRENDER_API explicit Image(uint32_t const *data);
		
		GLRENDER_API Color* getImageData();
		
		GLRENDER_API Color* getRow(size_t row);
		
		/// Get the pixel at the given x, y position in the image, starting from the top-left, left to right, top to bottom
		GLRENDER_API Color& getPixel(size_t x, size_t y);
		
		/// Get the index into the imageData array based on the x, y position of the pixel given
		GLRENDER_API size_t index(size_t x, size_t y) const;
		
		GLRENDER_API void expand(size_t xAmount, size_t yAmount, Color const &newPixelsColor);
		
		GLRENDER_API void copyFrom(Image const &src, size_t xOffset, size_t yOffset);
		
		std::vector<Color>  m_imageData = {};
		size_t              m_width = 0;
		size_t              m_height = 0;
		char                m_bitDepth = 8;
	};
	
	struct ImageOperation
	{
		GLRENDER_API virtual ~ImageOperation();
		GLRENDER_API virtual void run(Image &image) = 0;
	};
	
	struct FillOperation : public ImageOperation
	{
		GLRENDER_API ~FillOperation() override;
		GLRENDER_API explicit FillOperation(Color const &fillColor) : m_fillColor(fillColor) {}
		GLRENDER_API void run(Image &image) override;
		
		Color m_fillColor = {};
	};
	
	struct ReplaceColorOperation : public ImageOperation
	{
		GLRENDER_API ~ReplaceColorOperation() override;
		GLRENDER_API ReplaceColorOperation(Color const &src, Color const &dst) : m_src(src), m_dst(dst) {}
		GLRENDER_API void run(Image &image) override;
		
		Color m_src = {};
		Color m_dst = {};
	};
}
