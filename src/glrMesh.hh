#pragma once

#include "export.hh"

#include <commons/math/vec2.hh>
#include <initializer_list>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <array>

namespace glr
{
	struct QuadUVs
	{
		vec2<float> m_upperLeft = {};
		vec2<float> m_lowerLeft = {};
		vec2<float> m_upperRight = {};
		vec2<float> m_lowerRight = {};
	};
	
	//TODO support index buffer
	struct Mesh
	{
		Mesh() = delete;
		
		GLRENDER_API ~Mesh();
		
		//Base constructors, all others should be able to delegate to these
		GLRENDER_API Mesh(float const *verts, size_t vertsSize);
		GLRENDER_API Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize);
		GLRENDER_API Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize, float const *normals, size_t normalsSize);
		
		//float
		GLRENDER_API explicit Mesh(std::vector<float> const &verts);
		GLRENDER_API Mesh(std::vector<float> const &verts, std::vector<float> const &uvs);
		GLRENDER_API Mesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals);
		GLRENDER_API Mesh(std::initializer_list<float> const &verts);
		GLRENDER_API Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs);
		GLRENDER_API Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs, std::initializer_list<float> const &normals);
		
		template <size_t N> GLRENDER_API explicit Mesh(std::array<float, N> const &verts) : Mesh(verts.data(), verts.size()) {}
		template <size_t N> GLRENDER_API Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size()) {}
		template <size_t N> GLRENDER_API Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs, std::array<float, N> const &normals) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size()) {}
		
		//uint8, raw data read out of files
		GLRENDER_API explicit Mesh(std::vector<uint8_t> const &verts);
		GLRENDER_API Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs);
		GLRENDER_API Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs, std::vector<uint8_t> const &normals);
		GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts);
		GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs);
		GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs, std::initializer_list<uint8_t> const &normals);
		
		template <size_t N> GLRENDER_API explicit Mesh(std::array<uint8_t, N> const &verts) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4) {}
		template <size_t N> GLRENDER_API Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4) {}
		template <size_t N> GLRENDER_API Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs, std::array<uint8_t, N> const &normals) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4, reinterpret_cast<float const*>(normals.data()), normals.size() / 4) {}
		
		GLRENDER_API Mesh(Mesh &other);
		GLRENDER_API Mesh& operator=(Mesh other);
		GLRENDER_API Mesh(Mesh &&other) noexcept;
		GLRENDER_API Mesh& operator=(Mesh &&other) noexcept;
		
		GLRENDER_API void use() const;
		
		uint32_t m_vao = 0;
		uint32_t m_vboV = 0;
		uint32_t m_vboU = 0;
		uint32_t m_vboN = 0;
		uint32_t m_vboI = 0;
		size_t m_numVerts = 0;
		bool m_hasVerts = false;
		bool m_hasUVs = false;
		bool m_hasNormals = false;
	
	private:
		int32_t p_vertexStride = 3 * sizeof(float);
		int32_t p_uvStride = 2 * sizeof(float);
		int32_t p_normalStride = 3 * sizeof(float);
	};
}
