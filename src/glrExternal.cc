#include "glrExternal.hh"

#include <glad/gl.hh>

namespace GLRender
{
	void pixelStoreiPack(int i)
	{
		glPixelStorei(GL_PACK_ALIGNMENT, i);
	}
	
	void pixelStoreiUnpack(int i)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, i);
	}
	
	std::vector<uint8_t> getPixels(uint32_t width, uint32_t height)
	{
		std::vector<uint8_t> out;
		glReadPixels(0, 0, (GLsizei)width, (GLsizei)height, GL_RGB, GL_UNSIGNED_BYTE, out.data());
		return out;
	}
}
