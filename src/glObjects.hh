#pragma once

#include "export.hh"

#include <string>
#include <vector>
#include <memory>

namespace GLRender
{
	/// An on-VRAM OpenGL texture
	struct Texture
	{
		
	};
	
	/// An OpenGL frag/vert, geometry, or compute shader
	struct Shader
	{
		
	};
	
	enum struct FramebufferAttachment
	{
		Color, Alpha, Depth, Stencil,
	};
	
	/// An OpenGL framebuffer
	struct Framebuffer
	{
		Framebuffer() = delete;
		GLRENDER_API Framebuffer(uint32_t width, uint32_t height, std::initializer_list<FramebufferAttachment> const &options, std::string const &name);
		GLRENDER_API ~Framebuffer();
		GLRENDER_API Framebuffer(Framebuffer &other);
		GLRENDER_API Framebuffer& operator=(Framebuffer other);
		GLRENDER_API Framebuffer(Framebuffer &&other) noexcept;
		GLRENDER_API Framebuffer& operator=(Framebuffer &&other);
		
		GLRENDER_API void use();
		GLRENDER_API void bind(FramebufferAttachment type, uint32_t target);
		GLRENDER_API void regenerate(uint32_t width, uint32_t height);
		
		uint32_t m_handle = 0;
		uint32_t m_colorHandle = 0;
		uint32_t m_depthHandle = 0;
		uint32_t m_stencilHandle = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_hasColor = false;
		bool m_hasDepth = false;
		bool m_hasAlpha = false;
		bool m_hasStencil = false;
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
	
	/// An on-VRAM atlas of stitched together images as one OpenGL texture
	struct Atlas
	{
		
	};
	
	namespace Postprocessing
	{
		/// An OpenGL program that alters the image in an OpenGL framebuffer
		struct PostPass
		{
			GLRENDER_API virtual ~PostPass() = default;
			
			/// Process the pixels held by 'in', and write the result to 'out'
			GLRENDER_API virtual void process(Framebuffer &out, Framebuffer &in) = 0;
			
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
			GLRENDER_API bool empty();
		
		private:
			std::vector<std::shared_ptr<PostPass>> p_postOrder;
		};
	}
}
