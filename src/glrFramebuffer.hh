#pragma once

#include "export.hh"
#include "glrEnums.hh"

#include <string>
#include <vector>
#include <memory>

namespace glr
{
	/// An OpenGL framebuffer
	struct Framebuffer
	{
		Framebuffer() = delete;
		GLRENDER_API Framebuffer(uint32_t width, uint32_t height, std::initializer_list<Attachment> const &options, std::string const &name);
		GLRENDER_API ~Framebuffer();
		GLRENDER_API Framebuffer(Framebuffer &other);
		GLRENDER_API Framebuffer& operator=(Framebuffer other);
		GLRENDER_API Framebuffer(Framebuffer &&other) noexcept;
		GLRENDER_API Framebuffer& operator=(Framebuffer &&other) noexcept;
		
		GLRENDER_API void use() const;
		GLRENDER_API void bind(Attachment type, uint32_t target) const;
		GLRENDER_API void regenerate(uint32_t width, uint32_t height);
		
		uint32_t    m_handle = 0;
		uint32_t    m_colorHandle = 0;
		uint32_t    m_depthHandle = 0;
		uint32_t    m_stencilHandle = 0;
		uint32_t    m_width = 0;
		uint32_t    m_height = 0;
		bool        m_hasColor = false;
		bool        m_hasDepth = false;
		bool        m_hasAlpha = false;
		bool        m_hasStencil = false;
		std::string m_name;
	
	private:
		void createFBO();
		void clearFBO();
	};
	
	struct FramebufferPool
	{
		FramebufferPool() = delete;
		FramebufferPool(FramebufferPool const &other) = delete;
		FramebufferPool(FramebufferPool const &&other) = delete;
		GLRENDER_API FramebufferPool(size_t alloc, uint32_t width, uint32_t height);
		
		GLRENDER_API std::shared_ptr<Framebuffer> getNextAvailableFBO(uint32_t width, uint32_t height);
		GLRENDER_API void onResize(uint32_t width, uint32_t height);
	
	private:
		std::vector<std::shared_ptr<Framebuffer>> p_pool;
	};
}
