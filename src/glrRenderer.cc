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
	Renderer::Renderer(GLLoadFunc loadFunc, uint32_t contextWidth, uint32_t contextHeight)
	{
		gladLoadGL((GLADloadfunc)loadFunc);
		this->m_fboPool = std::make_unique<FramebufferPool>(2, contextWidth, contextHeight);
		this->p_fboA = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA}, "Ping");
		this->p_fboB = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR, Attachment::ALPHA}, "Pong");
		this->p_scratch = std::make_unique<Framebuffer>(contextWidth, contextHeight, std::initializer_list<Attachment>{Attachment::COLOR}, "Scratch");
		this->p_fullscreenQuad = std::make_unique<Mesh>();
		this->p_shaderTransfer = std::make_unique<Shader>();
	}
	
	Renderer::~Renderer()
	{
		this->p_fboA.reset();
		this->p_fboB.reset();
		this->p_scratch.reset();
		this->p_fullscreenQuad.reset();
		this->p_shaderTransfer.reset();
		this->p_globalPostStack.reset();
		this->p_layerPostStack.clear();
	}
	
	void Renderer::onContextResize(uint32_t width, uint32_t height)
	{
		this->useBackBuffer();
		glViewport(0, 0, (int)width, (int)height);
		this->m_fboPool->onResize(width, height);
	}
	
	void Renderer::setGlobalPostStack(std::shared_ptr<PostStack> stack)
	{
		this->p_globalPostStack = std::move(stack);
	}
	
	void Renderer::setLayerPostStack(uint64_t layer, std::shared_ptr<PostStack> stack)
	{
		this->p_layerPostStack[layer] = std::move(stack);
	}
	
	void Renderer::useBackBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void Renderer::render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection)
	{
		if(renderList.empty()) return;
		this->p_view = view;
		this->p_projection = projection;
		uint64_t curTexture = renderList[0].m_textureID;
		glBindTextureUnit(0, curTexture);
		if(this->p_layerPostStack.empty()) //No postprocessing
		{
			this->pingPong();
			for(size_t i = 0; i < renderList.size(); i++)
			{
				auto const &entry = renderList[i];
				if(entry.m_textureID != curTexture)
				{
					curTexture = entry.m_textureID;
					glBindTextureUnit(0, curTexture);
				}
				this->drawRenderable(entry);
			}
		}
		else
		{
			this->p_scratch->use();
			this->clearCurrentFramebuffer();
			this->pingPong();
			bool bind = false;
			size_t prevLayer = renderList[0].m_layer;
			for(size_t i = 0; i < renderList.size(); i++)
			{
				auto const &entry = renderList[i];
				if(entry.m_textureID != curTexture)
				{
					bind = true;
					curTexture = entry.m_textureID;
				}
				if(i == 0)
				{
					if(bind)
					{
						glBindTextureUnit(0, curTexture);
					}
					this->drawRenderable(entry);
				}
				else if(i == renderList.size() - 1)
				{
					if(entry.m_layer != prevLayer)
					{
						this->postProcessLayer(prevLayer);
						this->drawToScratch();
						this->pingPong();
						glBindTextureUnit(0, curTexture);
					}
					if(bind)
					{
						glBindTextureUnit(0, curTexture);
					}
					this->drawRenderable(entry);
					this->postProcessLayer(entry.m_layer);
					this->drawToScratch();
				}
				else
				{
					if(entry.m_layer != prevLayer)
					{
						this->postProcessLayer(prevLayer);
						this->drawToScratch();
						this->pingPong();
						glBindTextureUnit(0, curTexture);
					}
					if(bind)
					{
						glBindTextureUnit(0, curTexture);
					}
					this->drawRenderable(entry);
				}
				prevLayer = entry.m_layer;
				bind = false;
			}
			this->scratchToPingPong();
		}
		if(this->p_globalPostStack && !this->p_globalPostStack->empty())
		{
			this->postProcessGlobal();
		}
		this->drawToBackBuffer();
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
	
	void Renderer::postProcessLayer(uint64_t layer)
	{
		
	}
	
	void Renderer::postProcessGlobal()
	{
		
	}
	
	void Renderer::drawToBackBuffer()
	{
		this->p_fullscreenQuad->use();
		this->useBackBuffer();
		this->clearCurrentFramebuffer();
		this->p_shaderTransfer->use();
		this->p_curFBO.get() ? this->p_fboA->bind(Attachment::COLOR, 0) : this->p_fboB->bind(Attachment::COLOR, 0);
		draw(DrawMode::TRISTRIPS, this->p_fullscreenQuad->m_numVerts);
	}
	
	void Renderer::drawToScratch()
	{
		this->p_fullscreenQuad->use();
		this->p_scratch->use();
		this->p_shaderTransfer->use();
		this->p_curFBO.get() ? this->p_fboA->bind(Attachment::COLOR, 0) : this->p_fboB->bind(Attachment::COLOR, 0);
		draw(DrawMode::TRISTRIPS, this->p_fullscreenQuad->m_numVerts);
	}
	
	void Renderer::scratchToPingPong()
	{
		this->p_fullscreenQuad->use();
		this->pingPong();
		this->p_shaderTransfer->use();
		this->p_scratch->bind(Attachment::COLOR, 0);
		draw(DrawMode::TRISTRIPS, this->p_fullscreenQuad->m_numVerts);
	}
	
	void Renderer::drawRenderable(const Renderable &entry)
	{
		
	}
}
