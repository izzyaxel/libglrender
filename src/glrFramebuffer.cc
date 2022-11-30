#include "glrFramebuffer.hh"

#include <algorithm>
#include <glad/gl.hh>

namespace GLRender
{
	Framebuffer::Framebuffer(uint32_t width, uint32_t height, const std::initializer_list<Attachment> &options, const std::string &name)
	{
		this->m_width = width;
		this->m_height = height;
		for(auto const &option : options)
		{
			switch(option)
			{
				case Attachment::COLOR: this->m_hasColor = true; break;
				case Attachment::ALPHA: this->m_hasAlpha = true; break;
				case Attachment::DEPTH: this->m_hasDepth = true; break;
				case Attachment::STENCIL: this->m_hasStencil = true; break;
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
	
	Framebuffer::Framebuffer(Framebuffer &other)
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		
		this->m_colorHandle = other.m_colorHandle;
		other.m_colorHandle = 0;
		
		this->m_depthHandle = other.m_depthHandle;
		other.m_depthHandle = 0;
		
		this->m_stencilHandle = other.m_stencilHandle;
		other.m_stencilHandle = 0;
	}
	
	Framebuffer &Framebuffer::operator=(Framebuffer other)
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		
		this->m_colorHandle = other.m_colorHandle;
		other.m_colorHandle = 0;
		
		this->m_depthHandle = other.m_depthHandle;
		other.m_depthHandle = 0;
		
		this->m_stencilHandle = other.m_stencilHandle;
		other.m_stencilHandle = 0;
		return *this;
	}
	
	Framebuffer::Framebuffer(Framebuffer &&other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		
		this->m_colorHandle = other.m_colorHandle;
		other.m_colorHandle = 0;
		
		this->m_depthHandle = other.m_depthHandle;
		other.m_depthHandle = 0;
		
		this->m_stencilHandle = other.m_stencilHandle;
		other.m_stencilHandle = 0;
	}
	
	Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept
	{
		this->m_handle = other.m_handle;
		other.m_handle = 0;
		
		this->m_colorHandle = other.m_colorHandle;
		other.m_colorHandle = 0;
		
		this->m_depthHandle = other.m_depthHandle;
		other.m_depthHandle = 0;
		
		this->m_stencilHandle = other.m_stencilHandle;
		other.m_stencilHandle = 0;
		return *this;
	}
	
	void Framebuffer::use() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->m_handle);
	}
	
	void Framebuffer::bind(Attachment type, uint32_t target) const
	{
		switch(type)
		{
			case Attachment::COLOR: glBindTextureUnit(target, this->m_colorHandle); break;
			case Attachment::DEPTH: glBindTextureUnit(target, this->m_depthHandle); break;
			case Attachment::STENCIL: glBindTextureUnit(target, this->m_stencilHandle); break;
			default: break;
		}
	}
	
	void Framebuffer::regenerate(uint32_t width, uint32_t height)
	{
		this->m_width = width;
		this->m_height = height;
		this->clearFBO();
		this->createFBO();
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
	
	FramebufferPool::FramebufferPool(size_t alloc, uint32_t width, uint32_t height)
	{
		this->p_pool.resize(alloc);
		for(size_t i = 0; i < alloc; i++)
		{
			this->p_pool[i] = std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(i));
		}
	}
	
	std::shared_ptr<Framebuffer> FramebufferPool::getNextAvailableFBO(uint32_t width, uint32_t height)
	{
		std::shared_ptr<Framebuffer> out;
		for(auto &fbo : this->p_pool)
		{
			if(fbo.use_count() == 1)
			{
				if(fbo->m_width != width || fbo->m_height != height)
				{
					fbo = std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(this->p_pool.size() + 1));
				}
				fbo->use();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				return fbo;
			}
		}
		this->p_pool.push_back(std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA, Attachment::DEPTH}, "Pool " + std::to_string(this->p_pool.size() + 1)));
		this->p_pool.back()->use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return this->p_pool.back();
	}
	
	void FramebufferPool::onResize(uint32_t width, uint32_t height)
	{
		for(auto const &fbo : this->p_pool)
		{
			fbo->regenerate(width, height);
		}
	}
}
