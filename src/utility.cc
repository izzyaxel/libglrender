#include "utility.hh"

#include <commons/misc.hh>
#include <numeric>

namespace GLRender
{
	constexpr uint8_t s_ui8Max = std::numeric_limits<uint8_t>::max();
	constexpr uint16_t s_ui16Max = std::numeric_limits<uint16_t>::max();

/// ===Color=====================================================================================================================================================================///
	Color Color::operator+(Color const &other) const
	{
		Color out;
		out.p_red = this->p_red + other.p_red;
		out.p_green = this->p_green + other.p_green;
		out.p_blue = this->p_blue + other.p_blue;
		out.p_alpha = this->p_alpha + other.p_alpha;
		return out;
	}
	
	bool Color::operator==(Color const &other) const
	{
		return this->p_red == other.p_red && this->p_green == other.p_green && this->p_blue == other.p_blue && this->p_alpha == other.p_alpha;
	}
	
	void Color::fromRGBf(float r, float g, float b)
	{
		this->p_red = (colorFmt)(bound<float>(r, 0.0f, 1.0f) * s_ui16Max);
		this->p_green = (colorFmt)(bound<float>(g, 0.0f, 1.0f) * s_ui16Max);
		this->p_blue = (colorFmt)(bound<float>(b, 0.0f, 1.0f) * s_ui16Max);
	}
	
	void Color::fromRGBAf(float r, float g, float b, float a)
	{
		this->p_red = (colorFmt)(bound<float>(r, 0.0f, 1.0f) * s_ui16Max);
		this->p_green = (colorFmt)(bound<float>(g, 0.0f, 1.0f) * s_ui16Max);
		this->p_blue = (colorFmt)(bound<float>(b, 0.0f, 1.0f) * s_ui16Max);
		this->p_alpha = (colorFmt)(bound<float>(a, 0.0f, 1.0f) * s_ui16Max);
	}
	
	void Color::fromRGBui8(uint8_t r, uint8_t g, uint8_t b)
	{
		this->p_red = (colorFmt)(r * 256);
		this->p_green = (colorFmt)(g * 256);
		this->p_blue = (colorFmt)(b * 256);
	}
	
	void Color::fromRGBAui8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		this->p_red = (colorFmt)(r * 256);
		this->p_green = (colorFmt)(g * 256);
		this->p_blue = (colorFmt)(b * 256);
		this->p_alpha = (colorFmt)(a * 256);
	}
	
	void Color::fromRGBui16(uint16_t r, uint16_t g, uint16_t b)
	{
		this->p_red = r;
		this->p_green = g;
		this->p_blue = b;
	}
	
	void Color::fromRGBAui16(uint16_t r, uint16_t g, uint16_t b, uint16_t a)
	{
		this->p_red = r;
		this->p_green = g;
		this->p_blue = b;
		this->p_alpha = a;
	}
	
	void Color::fromHex(uint32_t hex)
	{
		this->p_alpha = (colorFmt)(((hex & 0xFF000000) >> 24) * 256);
		this->p_red = (colorFmt)(((hex & 0x00FF0000) >> 16) * 256);
		this->p_green = (colorFmt)(((hex & 0x0000FF00) >> 8) * 256);
		this->p_blue = (colorFmt)((hex & 0x000000FF) * 256);
	}
	
	void Color::fromWeb(std::string const &color)
	{
		if(color.empty() || color[0] != '#')
		{
			printf("Color::fromWeb: Invalid color formatting.\n");
			return;
		}
		uint32_t conv = (uint32_t)(std::strtol(color.data() + 1, nullptr, 16));
		switch(color.size())
		{
			default:
				printf("Color::fromWeb: Invalid color formatting.\n");
				break;
			case 4:
				this->p_blue = (colorFmt)((conv & 0x00F) * 256);
				this->p_green = (colorFmt)((conv & 0x0F0 << 4) * 256);
				this->p_red = (colorFmt)((conv & 0xF00 << 8) * 256);
				break;
			case 5:
				this->p_alpha = (colorFmt)((conv & 0x000F) * 256);
				this->p_blue = (colorFmt)((conv & 0x00F0 << 4) * 256);
				this->p_green = (colorFmt)((conv & 0x0F00 << 8) * 256);
				this->p_red = (colorFmt)((conv & 0xF000 << 12) * 256);
				break;
			case 7:
				this->p_red = (colorFmt)((conv & 0xFF000000 << 16) * 256);
				this->p_green = (colorFmt)((conv & 0xFF000000 << 8) * 256);
				this->p_blue = (colorFmt)((conv & 0xFF000000) * 256);
				break;
			case 9:
				this->p_red = (colorFmt)((conv & 0xFF000000 << 24) * 256);
				this->p_green = (colorFmt)((conv & 0xFF000000 << 16) * 256);
				this->p_blue = (colorFmt)((conv & 0xFF000000 << 8) * 256);
				this->p_alpha = (colorFmt)((conv & 0xFF000000) * 256);
				break;
		}
	}
	
	vec3<float> Color::asRGBf() const
	{
		return {(float)this->p_red / s_ui16Max, (float)this->p_green / s_ui16Max, (float)this->p_blue / s_ui16Max};
	}
	
	vec4<float> Color::asRGBAf() const
	{
		return {(float)this->p_red / s_ui16Max, (float)this->p_green / s_ui16Max, (float)this->p_blue / s_ui16Max, (float)this->p_alpha / s_ui16Max};
	}
	
	vec3<uint8_t> Color::asRGBui8() const
	{
		return {(uint8_t)(this->p_red / 256), (uint8_t)(this->p_green / 256), (uint8_t)(this->p_blue / 256)};
	}
	
	vec4<uint8_t> Color::asRGBAui8() const
	{
		return {(uint8_t)(this->p_red / 256), (uint8_t)(this->p_green / 256), (uint8_t)(this->p_blue / 256), (uint8_t)(this->p_alpha / 256)};
	}
	
	vec3<uint16_t> Color::asRGBui16() const
	{
		return {this->p_red, this->p_green, this->p_blue};
	}
	
	vec4<uint16_t> Color::asRGBAui16() const
	{
		return {this->p_red, this->p_green, this->p_blue, this->p_alpha};
	}
	
	uint32_t Color::asHex() const
	{
		return (uint8_t)(this->p_alpha / 256) << 24 | (uint8_t)(this->p_red / 256) << 16 | (uint8_t)(this->p_green / 256) << 8 | (uint8_t)(this->p_blue / 256);
	}
	
	std::string Color::asWeb() const
	{
		std::stringstream ss;
		ss << std::hex << ((uint8_t)(this->p_red / 256) << 24 | (uint8_t)(this->p_green / 256) << 16 | (uint8_t)(this->p_blue / 256) << 8 | (uint8_t)(this->p_alpha / 256));
		std::string ssc = ss.str();
		return std::string{"#"} + ssc;
	}
	
/// ===Image=====================================================================================================================================================================///
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
	
/// ===ImageOperations=============================================================================================================================================================///
	void FillOperation::run(Image &image)
	{
		for(auto &pix : image.m_imageData) pix = this->m_fillColor;
	}
	
	void ReplaceColorOperation::run(Image &image)
	{
		for(auto &pix : image.m_imageData) if(pix == this->m_src) pix = this->m_dst;
	}
}
