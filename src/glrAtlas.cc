#include "glrAtlas.hh"

#include <glad/gl.h>
#include <algorithm>

namespace GLRender
{
	void Atlas::addTile(std::string const &name, std::vector<uint8_t> const &tileData)
	{
		if(this->contains(name))
		{
			printf("Atlas error: Atlas already contains a file with the name %s\n", name.c_str());
			return;
		}
		if(this->p_finalized)
		{
			printf("Atlas error: Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize\n");
			return;
		}
		if(tileData.empty())
		{
			printf("Atlas error: Tile data is empty\n");
			return;
		}
		/*PNG decoded = decodePNG(tileData); //TODO take in raw image data
		TextureColorFormat f = TextureColorFormat::RGB;
		switch(decoded.m_colorFormat)
		{
			case PNG::COLOR_FMT_GREY:
				f = TextureColorFormat::GREY;
				break;
			case PNG::COLOR_FMT_RGB:
				f = TextureColorFormat::RGB;
				break;
			case PNG::COLOR_FMT_RGBA:
				f = TextureColorFormat::RGBA;
				break;
		}*/
		//this->p_atlas.push_back(AtlasImg{name, std::move(decoded.m_imageData), f, vec2<uint32_t>{0, 0}, decoded.m_width, decoded.m_height});
	}
	
	void Atlas::addTile(std::string const &name, TextureColorFormat fmt, std::vector<uint8_t> &&tileData, uint32_t width, uint32_t height)
	{
		if(this->contains(name))
		{
			printf("Atlas error: Atlas already contains a tile with the name %s\n", name.c_str());
			return;
		}
		if(this->p_finalized)
		{
			printf("Atlas error: Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize\n");
			return;
		}
		if(tileData.empty())
		{
			printf("Atlas error: Tile data is empty\n");
			return;
		}
		this->p_atlas.push_back(AtlasImg{name, std::move(tileData), fmt, vec2<uint32_t>{0, 0}, width, height});
	}
	
	Atlas::QuadUVs Atlas::getUVsForTile(std::string const &name)
	{
		if(!this->p_finalized || !this->contains(name))
		{
			return QuadUVs{};
		}
		vec2<uint32_t> location{};
		uint32_t width = 0, height = 0;
		for(auto &tile : this->p_atlas)
		{
			if(tile.m_name == name)
			{
				location = tile.m_location;
				width = tile.m_width;
				height = tile.m_height;
				break;
			}
		}
		vec2<float> ll = vec2<float>{(float)location.x(), (float)location.y()};
		vec2<float> ul = vec2<float>{(float)location.x(), (float)(location.y() + height)};
		vec2<float> lr = vec2<float>{(float)(location.x() + width), (float)location.y()};
		vec2<float> ur = vec2<float>{(float)(location.x() + width), (float)(location.y() + height)};
		ll = ll / this->p_atlasDims;
		ul = ul / this->p_atlasDims;
		lr = lr / this->p_atlasDims;
		ur = ur / this->p_atlasDims;
		return QuadUVs{ul, ll, ur, lr};
	}
	
	vec2<double> Atlas::getTileDimensions(std::string const &name)
	{
		if(!this->contains(name)) return {0.0f, 0.0f};
		for(auto const &tile : this->p_atlas)
		{
			if(tile.m_name == name)
			{
				return vec2<double>{(float)tile.m_width, (float)tile.m_height};
			}
		}
		return vec2<double>{0.0f, 0.0f};
	}
	
	void Atlas::use(uint32_t target) const
	{
		if(!this->p_finalized)
		{
			printf("Atlas error: Trying to bind atlas before it's been finalized\n");
		}
		else
		{
			AR::getTexture(this->p_texID)->use(target); //TODO decouple from Umbra
		}
	}
	
	bool Atlas::contains(std::string const &tileName)
	{
		for(auto const &img : this->p_atlas)
		{
			if(img.m_name == tileName)
			{
				return true;
			}
		}
		return false;
	}
	
	void Atlas::finalize(TextureColorFormat fmt)
	{
		if(this->p_finalized)
		{
			printf("Atlas error: Atlas has already been uploaded to the GPU, finalization failed\n");
			return;
		}
		if(this->p_atlas.empty())
		{
			printf("Atlas error: Atlas doesn't contain anything, finalization failed\n");
			return;
		}
		BSPLayout<uint32_t> layout; //TODO bring in BSPs
		std::sort(this->p_atlas.begin(), this->p_atlas.end(), AtlasImg::comparator);
		for(auto &tile : this->p_atlas)
		{
			if(tile.m_width == 0 || tile.m_height == 0)
			{
				printf("Atlas error: Atlas encountered a tile with 0 width or height: %s finalization failed\n", tile.m_name.c_str());
				continue;
			}
			tile.m_location = layout.pack(tile.m_width, tile.m_height);
		}
		if(layout.height() == 0 || layout.width() == 0)
		{
			printf("Atlas error: After layout, this atlas would have 0 width or height, finalization failed\n");
			return;
		}
		this->p_texID = AR::newTexture(layout.width(), layout.height(), fmt, FilterMode::NEAREST);
		AR::getTexture(this->p_texID)->clear();
		for(auto &tile : this->p_atlas)
		{
			AR::getTexture(this->p_texID)->subImage(tile.m_data.data(), tile.m_width, tile.m_height, tile.m_location.x(), tile.m_location.y(), tile.m_fmt);
		}
		this->p_atlasDims = {(float)layout.width(), (float)layout.height()};
		this->p_finalized = true;
	}
}
