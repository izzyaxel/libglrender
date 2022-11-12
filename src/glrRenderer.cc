#include "glrRenderer.hh"

#include <glad/gl.h>
#include <commons/math/quaternion.hh>

namespace GLRender
{
	uint32_t Renderer::s_workSizeX = 40;
	uint32_t Renderer::s_workSizeY = 20;
	
/// ===Data===========================================================================///
	
	std::string transferFrag =
R"(#version 450

in vec2 uv;
layout(binding = 0) uniform sampler2D tex;
out vec4 fragColor;

void main()
{
	fragColor = texture(tex, uv);
})";
	
	std::string transferVert =
R"(#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv_in;
out vec2 uv;

void main()
{
	uv = uv_in;
	gl_Position = vec4(pos, 1.0);
})";
	
	std::vector<float> fullscreenQuadVerts{1, -1, 0, 1, 1, 0, -1, -1, 0, -1, 1, 0};
	std::vector<float> fullscreenQuadUVs{1, 0, 1, 1, 0, 0, 0, 1};
/// ===Data===========================================================================///
	
	
	bool RenderList::renderableComparator(Renderable const &a, Renderable const &b)
	{
		if(a.m_texture && b.m_texture)
		{
			return (a.m_texture->m_handle > b.m_texture->m_handle) && (a.m_layer == b.m_layer) ? a.m_sublayer > b.m_sublayer : a.m_layer > b.m_layer;
		}
		else if(a.m_atlas && b.m_atlas)
		{
			return (a.m_atlas->m_atlasTexture->m_handle > b.m_atlas->m_atlasTexture->m_handle) && (a.m_layer == b.m_layer) ? a.m_sublayer > b.m_sublayer : a.m_layer > b.m_layer;
		}
		else
		{
			return (a.m_layer == b.m_layer) ? a.m_sublayer > b.m_sublayer : a.m_layer > b.m_layer;
		}
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
		this->p_fullscreenQuad = std::make_unique<Mesh>(fullscreenQuadVerts, fullscreenQuadUVs);
		this->p_shaderTransfer = std::make_unique<Shader>("Transfer Shader", transferVert, transferFrag);
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
		uint64_t curTexture = renderList[0].m_texture ? renderList[0].m_texture->m_handle : renderList[0].m_atlas->m_atlasTexture->m_handle;
		glBindTextureUnit(0, curTexture);
		if(this->p_layerPostStack.empty()) //No postprocessing
		{
			this->pingPong();
			for(size_t i = 0; i < renderList.size(); i++)
			{
				auto const &entry = renderList[i];
				if(entry.m_texture)
				{
					if(entry.m_texture->m_handle != curTexture)
					{
						curTexture = entry.m_texture->m_handle;
						glBindTextureUnit(0, curTexture);
					}
				}
				else if(entry.m_atlas)
				{
					if(entry.m_atlas->m_atlasTexture->m_handle != curTexture)
					{
						curTexture = entry.m_texture->m_handle;
						glBindTextureUnit(0, curTexture);
					}
				}
				else
				{
					continue;
				}
				this->drawRenderable(entry);
			}
		}
		else //Postprocess
		{
			this->p_scratch->use();
			this->clearCurrentFramebuffer();
			this->pingPong();
			bool bind = false;
			size_t prevLayer = renderList[0].m_layer;
			for(size_t i = 0; i < renderList.size(); i++)
			{
				auto const &entry = renderList[i];
				if(entry.m_texture)
				{
					if(entry.m_texture->m_handle != curTexture)
					{
						bind = true;
						curTexture = entry.m_texture->m_handle;
					}
				}
				else if(entry.m_atlas)
				{
					if(entry.m_atlas->m_atlasTexture->m_handle != curTexture)
					{
						bind = true;
						curTexture = entry.m_atlas->m_atlasTexture->m_handle;
					}
				}
				else
				{
					continue;
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
		for(auto const &stage : this->p_layerPostStack[layer]->getPasses())
		{
			if(stage->m_enabled)
			{
				this->pingPong();
				stage->process(this->p_curFBO.get() ? this->p_fboA : this->p_fboB, this->p_curFBO.get() ? this->p_fboB : this->p_fboA);
			}
		}
	}
	
	void Renderer::postProcessGlobal()
	{
		for(auto const &stage : this->p_globalPostStack->getPasses())
		{
			if(stage->m_enabled)
			{
				this->pingPong();
				stage->process(this->p_curFBO.get() ? this->p_fboA : this->p_fboB, this->p_curFBO.get() ? this->p_fboB : this->p_fboA);
			}
		}
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
		quat<float> rotation;
		rotation.fromAxial(vec3<float>{entry.m_axis}, degToRad<float>((float)entry.m_rotation));
		vec3<float> roundedPos = vec3<float>{vec2<float>{entry.m_pos}, 0};
		roundedPos.round();
		this->p_model = modelMatrix(roundedPos, rotation, vec3<float>(vec2<float>{entry.m_scale}, 1));
		this->p_mvp = modelViewProjectionMatrix(this->p_model, this->p_view, this->p_projection);
		entry.m_shader->use();
		entry.m_shader->sendMat4f("mvp", &this->p_mvp.data[0][0]);
		std::array<float, 12> quadVerts{0.5f, 0.5f, 0, -0.5f, 0.5f, 0, 0.5f, -0.5f, 0, -0.5f, -0.5f, 0};
		std::array<float, 8> quadUVs = {};
		if(entry.m_texture)
		{
			quadUVs = {1, 0, 0, 0, 1, 1, 0, 1};
		}
		else if(entry.m_atlas)
		{
			Atlas::QuadUVs uvs = entry.m_atlas->getUVsForTile(entry.m_name);
			quadUVs = {uvs.m_lowerRight.x(), uvs.m_lowerRight.y(), uvs.m_lowerLeft.x(), uvs.m_lowerLeft.y(), uvs.m_upperRight.x(), uvs.m_upperRight.y(), uvs.m_upperLeft.x(), uvs.m_upperLeft.y()};
		}
		
		Mesh mesh(quadVerts.data(), quadVerts.size(), quadUVs.data(), quadUVs.size());
		mesh.use();
		draw(DrawMode::TRISTRIPS, mesh.m_numVerts);
	}
	
	void Renderer::bindImage(uint32_t target, uint32_t const &handle, IO mode, GLColorFormat format)
	{
		glBindImageTexture(target, handle, 0, GL_FALSE, 0, (uint32_t)mode, (uint32_t)format);
	}
	
	void Renderer::startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize)
	{
		glDispatchCompute((uint32_t)(std::ceil((float)(contextSize.x()) / (float)workSize.x())), (uint32_t)(std::ceil((float)(contextSize.y()) / (float)workSize.y())), 1);
	}
}
