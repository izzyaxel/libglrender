#pragma once

#include "export.hh"
#include "glrEnums.hh"
#include "glrTexture.hh"

#include <commons/math/vec2.hh>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace GLRender
{
	/// An on-VRAM atlas of stitched together images as one OpenGL texture
	struct Atlas
	{
		struct QuadUVs
		{
			vec2<float> m_upperLeft = {};
			vec2<float> m_lowerLeft = {};
			vec2<float> m_upperRight = {};
			vec2<float> m_lowerRight = {};
		};
		
		GLRENDER_API ~Atlas();
		
		/// Add a new tile into this atlas
		/// \param name The name of the tile
		/// \param tileData Flat array of pixel data
		/// \param width The width of the new tile
		/// \param height The height of the new tile
		GLRENDER_API void addTile(std::string const &name, std::vector<uint8_t> const &tileData, TextureColorFormat format, uint32_t width, uint32_t height);
		
		/// Add a new tile into this atlas from raw pixel data
		GLRENDER_API void addTile(std::string const &name, TextureColorFormat fmt, std::vector<uint8_t> &&tileData, uint32_t width, uint32_t height);
		
		/// Get the UV coordinates in the atlas for the given tile
		/// \param id The ID of the tile
		/// \return UV coordinates
		GLRENDER_API QuadUVs getUVsForTile(std::string const &name);
		
		GLRENDER_API vec2<double> getTileDimensions(std::string const &name);
		
		/// Bind this atlas for rendering use
		GLRENDER_API void use(uint32_t target = 0) const;
		
		/// Check if this atlas contains a tile of the given name
		GLRENDER_API bool contains(std::string const &tileName);
		
		/// Create the atlas and send it to the GPU
		GLRENDER_API void finalize(TextureColorFormat fmt);
	
	private:
		struct AtlasImg
		{
			GLRENDER_API AtlasImg() = default;
			GLRENDER_API AtlasImg(std::string name, std::vector<uint8_t> data, TextureColorFormat fmt, vec2<uint32_t> location, uint32_t width, uint32_t height) :
					m_name(std::move(name)), m_data(std::move(data)), m_fmt(fmt), m_location(location), m_width(width), m_height(height) {}
			
			GLRENDER_API static inline bool comparator(AtlasImg const &a, AtlasImg const &b)
			{
				return a.m_height * a.m_width > b.m_height * b.m_width;
			}
			
			std::string m_name;
			std::vector<uint8_t> m_data = {};
			TextureColorFormat m_fmt = TextureColorFormat::RGBA;
			vec2<uint32_t> m_location = {};
			uint32_t m_width = 0;
			uint32_t m_height = 0;
		};
		
		vec2<float> p_atlasDims = {};
		std::vector<AtlasImg> p_atlas = {};
		std::shared_ptr<Texture> p_tex;
		bool p_finalized = false;
	};
}
