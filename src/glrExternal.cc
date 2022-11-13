#include "glrExternal.hh"

#include <glad/gl.h>

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
}
