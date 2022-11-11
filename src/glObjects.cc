#include "glObjects.hh"

#include <algorithm>
#include <glad/gl.h>

namespace GLRender
{
	Framebuffer::Framebuffer(uint32_t width, uint32_t height, const std::initializer_list<FramebufferAttachment> &options, const std::string &name)
	{
		this->m_width = width;
		this->m_height = height;
		for(auto const &option : options)
		{
			switch(option)
			{
				case FramebufferAttachment::Color: this->m_hasColor = true; break;
				case FramebufferAttachment::Alpha: this->m_hasAlpha = true; break;
				case FramebufferAttachment::Depth: this->m_hasDepth = true; break;
				case FramebufferAttachment::Stencil: this->m_hasStencil = true; break;
				default: break;
			}
		}
		this->m_name = name;
		this->createFBO();
	}
	
	Framebuffer::~Framebuffer()
	{
		this->clearFBO();
	}
	
	void Framebuffer::use()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->m_handle);
	}
	
	void Framebuffer::createFBO()
	{
		glCreateFramebuffers(1, &this->m_handle);
		this->use();
		glViewport(0, 0, (GLsizei)this->m_width, (GLsizei)this->m_height);
		glScissor(0, 0, (GLsizei)this->m_width, (GLsizei)this->m_height);
		if(this->m_hasColor) glCreateTextures(GL_TEXTURE_2D, 1, &this->m_colorHandle);
		if(this->m_hasDepth) glCreateTextures(GL_TEXTURE_2D, 1, &this->m_depthHandle);
		glTextureStorage2D(this->m_colorHandle, 1, this->m_hasAlpha ? GL_RGBA32F : GL_RGB32F, (GLsizei)this->m_width, (GLsizei)this->m_height);
		glNamedFramebufferTexture(this->m_handle, GL_COLOR_ATTACHMENT0, this->m_colorHandle, 0);
		if(this->m_hasDepth)
		{
			glTextureStorage2D(this->m_depthHandle, 1, GL_DEPTH_COMPONENT32F, (GLsizei)this->m_width, (GLsizei)this->m_height);
			glNamedFramebufferTexture(this->m_handle, GL_DEPTH_ATTACHMENT, this->m_depthHandle, 0);
		}
		std::vector<GLenum> drawBuffers;
		drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
		if(this->m_hasDepth) drawBuffers.emplace_back(GL_COLOR_ATTACHMENT1);
		glNamedFramebufferDrawBuffers(this->m_handle, static_cast<int32_t>(drawBuffers.size()), drawBuffers.data());
		GLenum error = glCheckNamedFramebufferStatus(this->m_handle, GL_FRAMEBUFFER);
		if(error != GL_FRAMEBUFFER_COMPLETE)
		{
			std::string er = "Framebuffer Creation Error: ";
			switch(error)
			{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: er += "incomplete attachment"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: er += "incomplete dimensions"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: er += "missing attachment"; break;
				case GL_FRAMEBUFFER_UNSUPPORTED: er += "Framebuffers are not supported"; break;
				default: break;
			}
			printf("%s\n", er.c_str());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void Framebuffer::clearFBO()
	{
		glDeleteFramebuffers(1, &this->m_handle);
		glDeleteTextures(1, &this->m_colorHandle);
		glDeleteTextures(1, &this->m_depthHandle);
	}
	
	namespace Postprocessing
	{
		void PostStack::add(std::shared_ptr<PostPass> pass)
		{
			this->p_postOrder.push_back(std::move(pass));
		}
		
		void PostStack::remove(std::shared_ptr<PostPass> const &pass)
		{
			this->p_postOrder.erase(std::find(this->p_postOrder.begin(), this->p_postOrder.end(), pass));
		}
		
		std::vector<std::shared_ptr<PostPass>> PostStack::getPasses()
		{
			return this->p_postOrder;
		}
		
		bool PostStack::empty()
		{
			return this->p_postOrder.empty();
		}
	}
}
