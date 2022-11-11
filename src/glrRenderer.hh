#pragma once

#include "glrPostProcessing.hh"
#include "glrColor.hh"
#include "glrMesh.hh"
#include "glrShader.hh"

#include <commons/math/vec2.hh>
#include <commons/math/vec3.hh>
#include <commons/math/mat4.hh>
#include <cstdint>
#include <functional>

namespace GLRender
{
	typedef void* (*GLLoadFunc)(const char *name);
	
	struct Renderable
	{
		GLRENDER_API Renderable(vec2<double> const &pos, vec2<double> const &scale, double rotation, vec3<double> const &axis, uint64_t atlasID, uint64_t shaderID, size_t layer, size_t sublayer, std::string name) :
				m_pos(pos), m_scale(scale), m_rotation(rotation), m_axis(axis), m_textureID(atlasID), m_shaderID(shaderID), m_layer(layer), m_sublayer(sublayer), m_name(std::move(name)) {}
				
		vec2<double> m_pos = {};
		vec2<double> m_scale = {};
		double m_rotation = 0.0f;
		vec3<double> m_axis = {0.0f, 0.0f, 1.0f};
		uint64_t m_textureID = 0;
		uint64_t m_shaderID = 0;
		size_t m_layer = 0;
		size_t m_sublayer = 0;
		char m_character = 0;
		Color m_color = {};
		std::string m_name;
	};
	
	struct RenderList
	{
		using Comparator = std::function<bool(Renderable const &a, Renderable const &b)>;
		
		GLRENDER_API static bool renderableComparator(Renderable const &a, Renderable const &b);
		GLRENDER_API Renderable& operator [](size_t index);
		GLRENDER_API void add(std::initializer_list<Renderable> const &renderables);
		GLRENDER_API void clear();
		GLRENDER_API bool empty() const;
		GLRENDER_API size_t size() const;
		GLRENDER_API void sort(Comparator const &cmp = RenderList::renderableComparator);
		
		std::vector<Renderable> m_list;
	};
	
	/// The OpenGL 4.5+ rendering engine
	struct Renderer
	{
		/// You're expected to be using a windowing library like SDL2, it will provide you
		/// with @param loadFunc
		GLRENDER_API explicit Renderer(GLLoadFunc loadFunc);
		GLRENDER_API ~Renderer();
		
		GLRENDER_API void setGlobalPostStack(PostStack const &stack);
		GLRENDER_API void setLayerPostStack(uint64_t layer, PostStack const &stack);
		GLRENDER_API void addGlobalPostStackToThisFrame(PostStack const &stack);
		GLRENDER_API void addLayerPostStackToThisFrame(uint64_t layer, PostStack const &stack);
		GLRENDER_API void useBackBuffer();
		GLRENDER_API void render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection, PostStack const &stack);
		GLRENDER_API void setClearColor(Color color);
		GLRENDER_API void clearCurrentFramebuffer();
		GLRENDER_API void setScissorTest(bool val);
		GLRENDER_API void setDepthTest(bool val);
		GLRENDER_API void setBlending(bool val);
		GLRENDER_API void setBlendMode(uint32_t src, uint32_t dst);
		GLRENDER_API void setCullFace(bool val);
		GLRENDER_API void setFilterMode(FilterMode mode);
		
		GLRENDER_API void bindImage(uint32_t target, uint32_t const &handle, IO mode, TextureColorFormat format);
		GLRENDER_API void startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize);
		GLRENDER_API void draw(DrawMode mode, size_t numElements);
		
	protected:
		void pingPong();
		
	private:
		struct Alternator
		{
			inline bool swap()
			{
				p_alt = !p_alt;
				return p_alt;
			}
			
			inline bool get() const
			{
				return p_alt;
			} //true: a false: b
		
		private:
			bool p_alt = true;
		};
		
		void postProcess();
		void postprocessGlobal(PostStack const &stack);
		void postProcessLayer(uint64_t layer, PostStack const &stack);
		void drawToScratch();
		void drawToBackBuffer();
		void scratchToPingPong();
		void drawRenderable(Renderable const &entry);
		
		Color p_clearColor = {};
		mat4x4<float> p_model = {};
		mat4x4<float> p_view = {};
		mat4x4<float> p_projection = {};
		mat4x4<float> p_mvp = {};
		std::unique_ptr<Framebuffer> p_fboA = nullptr;
		std::unique_ptr<Framebuffer> p_fboB = nullptr;
		std::unique_ptr<Framebuffer> p_scratch = nullptr;
		std::unique_ptr<Mesh> p_fullscreenQuad = nullptr;
		std::unique_ptr<Shader> p_shaderTransfer = nullptr;
		Alternator p_curFBO = {};
	};
}
