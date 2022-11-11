#include "glrRenderer.hh"

#include <glad/gl.h>

namespace GLRender
{
	bool RenderList::renderableComparator(Renderable const &a, Renderable const &b)
	{
		return (a.m_textureID > b.m_textureID) && (a.m_layer == b.m_layer) ? a.m_sublayer > b.m_sublayer : a.m_layer > b.m_layer;
	}
	
	Renderable& RenderList::operator [](size_t index)
	{
		return this->m_list[index];
	}
	
	void RenderList::add(std::initializer_list<Renderable> const &renderables)
	{
		this->m_list.insert(this->m_list.end(), renderables.begin(), renderables.end());
	}
	
	void RenderList::clear()
	{
		this->m_list.clear();
	}
	
	bool RenderList::empty() const
	{
		return this->m_list.empty();
	}
	
	size_t RenderList::size() const
	{
		return this->m_list.size();
	}
	
	void RenderList::sort(Comparator const &cmp)
	{
		std::sort(this->m_list.begin(), this->m_list.end(), cmp);
	}
	
/// ===Renderer========================================================================================================================================///
	Renderer::Renderer(GLLoadFunc loadFunc)
	{
		gladLoadGL((GLADloadfunc)loadFunc);
		//TODO set up members
	}
	
	Renderer::~Renderer()
	{
		this->p_fboA.reset();
		this->p_fboB.reset();
		this->p_scratch.reset();
	}
	
	void Renderer::setGlobalPostStack(PostStack const &stack)
	{
		
	}
	
	void Renderer::setLayerPostStack(uint64_t layer, PostStack const &stack)
	{
		
	}
	
	void Renderer::addGlobalPostStackToThisFrame(PostStack const &stack)
	{
		
	}
	
	void Renderer::addLayerPostStackToThisFrame(uint64_t layer, PostStack const &stack)
	{
		
	}
	
	void Renderer::useBackBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void Renderer::render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection, PostStack const &stack)
	{
		
	}
	
	void Renderer::setClearColor(Color color)
	{
		auto colorF = color.asRGBAf();
		glClearColor(colorF.r(), colorF.g(), colorF.b(), colorF.a());
		this->p_clearColor = color;
	}
	
	void Renderer::clearCurrentFramebuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void Renderer::setScissorTest(bool val)
	{
		val ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	}
	
	void Renderer::setDepthTest(bool val)
	{
		val ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}
	
	void Renderer::setBlending(bool val)
	{
		val ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}
	
	void Renderer::setBlendMode(uint32_t src, uint32_t dst)
	{
		glBlendFunc(src, dst);
	}
	
	void Renderer::setCullFace(bool val)
	{
		val ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}
	
	void Renderer::setFilterMode(FilterMode mode)
	{
		switch(mode)
		{
			case FilterMode::NEAREST:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			
			case FilterMode::BILINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			
			case FilterMode::TRILINEAR:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				break;
		}
	}
	
	void Renderer::bindImage(uint32_t target, uint32_t const &handle, IO mode, TextureColorFormat format)
	{
		glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
	}
	
	void Renderer::startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize)
	{
		glDispatchCompute((uint32_t)(std::ceil((float)(contextSize.x()) / (float)workSize.x())), (uint32_t)(std::ceil((float)(contextSize.y()) / (float)workSize.y())), 1);
	}
	
	void Renderer::draw(DrawMode mode, size_t numElements)
	{
		glDrawArrays((GLenum)mode, 0, (GLsizei)numElements);
	}
	
	void Renderer::pingPong()
	{
		this->p_curFBO.swap() ? this->p_fboA->use() : this->p_fboB->use();
		this->clearCurrentFramebuffer();
	}
	
	void Renderer::postProcess()
	{
		
	}
	
	void Renderer::postProcessLayer(uint64_t layer, const PostStack &stack)
	{
		
	}
	
	void Renderer::postprocessGlobal(const PostStack &stack)
	{
		
	}
	
	void Renderer::drawRenderable(const Renderable &entry)
	{
		
	}
}
