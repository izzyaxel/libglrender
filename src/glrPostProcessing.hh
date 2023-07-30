#pragma once

#include "glrFramebuffer.hh"

#include <vector>
#include <memory>

namespace glr
{
	/// An OpenGL program that alters the image in an OpenGL framebuffer
	struct PostPass
	{
		GLRENDER_API virtual ~PostPass() = default;
		
		/// Process the pixels held by 'in', and write the result to 'out'
		GLRENDER_API virtual void process(std::shared_ptr<Framebuffer> &out, std::shared_ptr<Framebuffer> &in) = 0;
		
		bool m_enabled = true;
		std::string m_name;
	
	protected:
		GLRENDER_API PostPass() = default;
	};
	
	struct PostStack
	{
		GLRENDER_API void add(std::shared_ptr<PostPass> pass);
		GLRENDER_API void remove(std::shared_ptr<PostPass> const &pass);
		GLRENDER_API std::vector<std::shared_ptr<PostPass>> getPasses();
		GLRENDER_API bool empty() const;
	
	private:
		std::vector<std::shared_ptr<PostPass>> p_postOrder;
	};
}
