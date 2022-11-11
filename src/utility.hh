#pragma once

#include "export.hh"

#include <commons/math/vec3.hh>
#include <commons/math/vec4.hh>
#include <cstdint>
#include <string>
#include <memory>
#include <cstddef>
#include <vector>

namespace GLRender
{
	struct Color
	{
		using colorFmt = uint16_t;
		
		GLRENDER_API Color() = default;
		GLRENDER_API Color operator + (Color const &other) const;
		GLRENDER_API bool operator == (Color const &other) const;
		
		GLRENDER_API void fromRGBf(float r, float g, float b);
		GLRENDER_API void fromRGBAf(float r, float g, float b, float a);
		GLRENDER_API void fromRGBui8(uint8_t r, uint8_t g, uint8_t b);
		GLRENDER_API void fromRGBAui8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		GLRENDER_API void fromRGBui16(uint16_t r, uint16_t g, uint16_t b);
		GLRENDER_API void fromRGBAui16(uint16_t r, uint16_t g, uint16_t b, uint16_t a);
		GLRENDER_API void fromHex(uint32_t hex);
		GLRENDER_API void fromWeb(std::string const &color);
		
		/// 0-1 float RGB representation
		GLRENDER_API vec3<float> asRGBf() const;
		
		/// 0-1 float RGBA representation
		GLRENDER_API vec4<float> asRGBAf() const;
		
		/// 0-255 RGB representation
		GLRENDER_API vec3<uint8_t> asRGBui8() const;
		
		/// 0-255 RGBA representation
		GLRENDER_API vec4<uint8_t> asRGBAui8() const;
		
		/// 0-65535 RGB representation
		GLRENDER_API vec3<uint16_t> asRGBui16() const;
		
		/// 0-65535 RGBA representation
		GLRENDER_API vec4<uint16_t> asRGBAui16() const;
		
		/// 0xAARRGGBB representation
		GLRENDER_API uint32_t asHex() const;
		
		/// #RRGGBBAA representation
		GLRENDER_API std::string asWeb() const;
	
	private:
		colorFmt p_red = 0;
		colorFmt p_green = 0;
		colorFmt p_blue = 0;
		colorFmt p_alpha = 1;
	};
	
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
		
		
		
		std::vector<Color>  m_imageData;
		size_t              m_width = 0;
		size_t              m_height = 0;
		char                m_bitDepth = 8;
	};
	
	struct ImageOperation
	{
		GLRENDER_API virtual ~ImageOperation() = default;
		GLRENDER_API virtual void run(Image &image) = 0;
	};
	
	struct FillOperation : public ImageOperation
	{
		GLRENDER_API explicit FillOperation(Color const &fillColor) : m_fillColor(fillColor) {}
		GLRENDER_API void run(Image &image) override;
		
		Color m_fillColor;
	};
	
	struct ReplaceColorOperation : public ImageOperation
	{
		GLRENDER_API ReplaceColorOperation(Color const &src, Color const &dst) : m_src(src), m_dst(dst) {}
		GLRENDER_API void run(Image &image) override;
		
		Color m_src = {};
		Color m_dst = {};
	};
}
