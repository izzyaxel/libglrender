#pragma once

#include "export.hh"
#include "glObjects.hh"
#include "utility.hh"

#include <commons/math/vec2.hh>
#include <commons/math/vec3.hh>
#include <commons/math/mat4.hh>
#include <cstdint>
#include <functional>

namespace GLRender
{
	/// Access type for compute shader imnage binding
	/// Mirrors OpenGL's enums
	enum struct IO
	{
		READ = 0x88B8, WRITE = 0x88B9, READWRITE = 0x88BA
	};
	
	/// Color format for compute shader image binding
	/// Mirrors OpenGL's enums
	enum struct ColorFormat
	{
		R32F = 0x822E,
		RGB8 = 0x8051,
		RGBA8 = 0x8058,
		RGB16 = 0x8054,
		RGBA16 = 0x805B,
		RGB32I = 0x8D83,
		RGBA32I = 0x8D82,
		RGB32UI = 0x8D71,
		RGBA32UI = 0x8D70,
		RGB16F = 0x881B,
		RGBA16F = 0x881A,
		RGB32F = 0x8815,
		RGBA32F = 0x8814,
		DEPTH32F = 0x8CAC,
	};
	
	/// Mode to draw a VAO in
	/// Mirrors OpenGL's enums
	enum struct DrawMode
	{
		TRIS = 0x0004,
		TRISTRIPS = 0x0005,
		TRIFANS = 0x0006,
		LINES = 0x0001,
		LINESTRIPS = 0x0003,
		LINELOOPS = 0x0002,
		POINTS = 0x0000,
	};
	
	enum struct FilterMode
	{
		NEAREST,
		BILINEAR,
		TRILINEAR
	};
	
	enum struct RenderableType
	{
		NORMAL,
		TEXT,
	};
	
	struct Renderable
	{
		GLRENDER_API Renderable(RenderableType type, vec2<double> const &pos, vec2<double> const &scale, double rotation, vec3<double> const &axis, uint64_t atlasID, uint64_t shaderID, size_t layer, size_t sublayer, std::string name) :
				m_type(type), m_pos(pos), m_scale(scale), m_rotation(rotation), m_axis(axis), m_atlasID(atlasID), m_shaderID(shaderID), m_layer(layer), m_sublayer(sublayer), m_name(std::move(name)) {}
		
		RenderableType m_type = RenderableType::NORMAL;
		vec2<double> m_pos = {};
		vec2<double> m_scale = {};
		double m_rotation = 0.0f;
		vec3<double> m_axis = {0.0f, 0.0f, 1.0f};
		uint64_t m_atlasID = 0;
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
	
	/// An OpenGL 4.6 rendering engine
	struct Renderer
	{
		GLRENDER_API Renderer();
		GLRENDER_API ~Renderer();
		
		GLRENDER_API void setGlobalPostStack(Postprocessing::PostStack const &stack);
		GLRENDER_API void setLayerPostStack(uint64_t layer, Postprocessing::PostStack const &stack);
		GLRENDER_API void addGlobalPostStackToThisFrame(Postprocessing::PostStack const &stack);
		GLRENDER_API void addLayerPostStackToThisFrame(uint64_t layer, Postprocessing::PostStack const &stack);
		GLRENDER_API void useBackBuffer();
		GLRENDER_API void render(RenderList renderList, mat4x4<float> const &view, mat4x4<float> const &projection, Postprocessing::PostStack const &stack);
		GLRENDER_API void setClearColor(Color color);
		GLRENDER_API void clearCurrentFramebuffer();
		GLRENDER_API void setScissorTest(bool val);
		GLRENDER_API void setDepthTest(bool val);
		GLRENDER_API void setBlending(bool val);
		GLRENDER_API void setBlendMode(uint32_t src, uint32_t dst);
		GLRENDER_API void setCullFace(bool val);
		GLRENDER_API void setFilterMode(FilterMode mode);
		
		GLRENDER_API void bindImage(uint32_t target, uint32_t const &handle, IO mode, ColorFormat format);
		GLRENDER_API void startComputeShader(vec2<uint32_t> const &contextSize, vec2<uint32_t> const &workSize);
		GLRENDER_API void draw(DrawMode mode, size_t numElements);
		
	protected:
		void pingPong();
		
	private:
		struct Alternator
		{
			inline bool swap()
			{
				p_alt = !p_alt; return p_alt;
			}
			
			inline bool get() const
			{
				return p_alt;
			} //true: a false: b
		
		private:
			bool p_alt = true;
		};
		
		void postprocess();
		void postprocessGlobal(Postprocessing::PostStack const &stack);
		void postProcessLayer(uint64_t layer, Postprocessing::PostStack const &stack);
		void drawRenderable(Renderable const &entry);
		
		Color p_clearColor;
		mat4x4<float> p_model;
		mat4x4<float> p_view;
		mat4x4<float> p_projection;
		mat4x4<float> p_mvp;
		std::unique_ptr<Framebuffer> p_fboA = nullptr;
		std::unique_ptr<Framebuffer> p_fboB = nullptr;
		std::unique_ptr<Framebuffer> p_scratch = nullptr;
		Alternator p_curFBO;
	};
}
