#include "glrMesh.hh"

#include <glad/gl.h>

namespace GLRender
{
	Mesh::~Mesh()
	{
		glDeleteBuffers(1, &this->m_vboV);
		glDeleteBuffers(1, &this->m_vboU);
		glDeleteBuffers(1, &this->m_vboN);
		glDeleteBuffers(1, &this->m_vboI);
		glDeleteVertexArrays(1, &this->m_vao);
	}
	
	Mesh::Mesh(float const *verts, size_t vertsSize)
	{
		if(vertsSize != 0)
		{
			this->m_hasVerts = true;
			this->m_numVerts = vertsSize / 3;
		}
		glCreateVertexArrays(1, &this->m_vao);
		glCreateBuffers(1, &this->m_vboV);
		
		glNamedBufferData(this->m_vboV, (GLsizeiptr)(vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 0, 0);
		glVertexArrayVertexBuffer(this->m_vao, 0, this->m_vboV, 0, this->p_vertexStride);
		glEnableVertexArrayAttrib(this->m_vao, 0);
		glVertexArrayAttribFormat(this->m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	}
	
	Mesh::Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize)
	{
		if(vertsSize != 0)
		{
			this->m_hasVerts = true;
			this->m_numVerts = vertsSize / 3;
		}
		if(uvsSize != 0) this->m_hasUVs = true;
		glCreateVertexArrays(1, &this->m_vao);
		glCreateBuffers(1, &this->m_vboV);
		glCreateBuffers(1, &this->m_vboU);
		
		glNamedBufferData(this->m_vboV, (GLsizeiptr)(vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 0, 0);
		glVertexArrayVertexBuffer(this->m_vao, 0, this->m_vboV, 0, this->p_vertexStride);
		glEnableVertexArrayAttrib(this->m_vao, 0);
		glVertexArrayAttribFormat(this->m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		
		glNamedBufferData(this->m_vboU, (GLsizeiptr)(uvsSize * sizeof(float)), uvs, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 1, 1);
		glVertexArrayVertexBuffer(this->m_vao, 1, this->m_vboU, 0, this->p_uvStride);
		glEnableVertexArrayAttrib(this->m_vao, 1);
		glVertexArrayAttribFormat(this->m_vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	}
	
	Mesh::Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize, float const *normals, size_t normalsSize)
	{
		if(vertsSize != 0)
		{
			this->m_hasVerts = true;
			this->m_numVerts = vertsSize / 3;
		}
		if(uvsSize != 0) this->m_hasUVs = true;
		if(normalsSize != 0) this->m_hasNormals = true;
		glCreateVertexArrays(1, &this->m_vao);
		glCreateBuffers(1, &this->m_vboV);
		glCreateBuffers(1, &this->m_vboU);
		glCreateBuffers(1, &this->m_vboN);
		
		glNamedBufferData(this->m_vboV, (GLsizeiptr)(vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 0, 0);
		glVertexArrayVertexBuffer(this->m_vao, 0, this->m_vboV, 0, this->p_vertexStride);
		glEnableVertexArrayAttrib(this->m_vao, 0);
		glVertexArrayAttribFormat(this->m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		
		glNamedBufferData(this->m_vboU, (GLsizeiptr)(uvsSize * sizeof(float)), uvs, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 1, 1);
		glVertexArrayVertexBuffer(this->m_vao, 1, this->m_vboU, 0, this->p_uvStride);
		glEnableVertexArrayAttrib(this->m_vao, 1);
		glVertexArrayAttribFormat(this->m_vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
		
		glNamedBufferData(this->m_vboN, (GLsizeiptr)(normalsSize * sizeof(float)), normals, GL_STATIC_DRAW);
		glVertexArrayAttribBinding(this->m_vao, 2, 2);
		glVertexArrayVertexBuffer(this->m_vao, 2, this->m_vboN, 0, this->p_normalStride);
		glEnableVertexArrayAttrib(this->m_vao, 2);
		glVertexArrayAttribFormat(this->m_vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	}
	
	Mesh::Mesh(std::vector<float> const &verts) : Mesh(verts.data(), verts.size()) {}
	Mesh::Mesh(std::vector<float> const &verts, std::vector<float> const &uvs) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size()) {}
	Mesh::Mesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size()) {}
	Mesh::Mesh(std::initializer_list<float> const &verts) : Mesh(verts.begin(), verts.size()) {}
	Mesh::Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs) : Mesh(verts.begin(), verts.size(), uvs.begin(), uvs.size()) {}
	Mesh::Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs, std::initializer_list<float> const &normals) : Mesh(verts.begin(), verts.size(), uvs.begin(), uvs.size(), normals.begin(), normals.size()) {}
	
	Mesh::Mesh(std::vector<uint8_t> const &verts) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4) {}
	Mesh::Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4) {}
	Mesh::Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs, std::vector<uint8_t> const &normals) : Mesh(reinterpret_cast<float const*>(verts.data()), verts.size() / 4, reinterpret_cast<float const*>(uvs.data()), uvs.size() / 4, reinterpret_cast<float const*>(normals.data()), normals.size() / 4) {}
	Mesh::Mesh(std::initializer_list<uint8_t> const &verts) : Mesh(reinterpret_cast<float const*>(verts.begin()), verts.size() / 4) {}
	Mesh::Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs) : Mesh(reinterpret_cast<float const*>(verts.begin()), verts.size() / 4, reinterpret_cast<float const*>(uvs.begin()), uvs.size() / 4) {}
	Mesh::Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs, std::initializer_list<uint8_t> const &normals) : Mesh(reinterpret_cast<float const*>(verts.begin()), verts.size() / 4, reinterpret_cast<float const*>(uvs.begin()), uvs.size() / 4, reinterpret_cast<float const*>(normals.begin()), normals.size() / 4) {}
	
	Mesh::Mesh(Mesh &other)
	{
		this->m_vao = other.m_vao;
		other.m_vao = 0;
		
		this->m_vboV = other.m_vboV;
		other.m_vboV = 0;
		
		this->m_vboU = other.m_vboU;
		other.m_vboU = 0;
		
		this->m_vboN = other.m_vboN;
		other.m_vboN = 0;
		
		this->m_vboI = other.m_vboI;
		other.m_vboI = 0;
		
		this->m_numVerts = other.m_numVerts;
		other.m_numVerts = 0;
		
		this->m_hasVerts = other.m_hasVerts;
		other.m_hasVerts = false;
		
		this->m_hasUVs = other.m_hasUVs;
		other.m_hasUVs = false;
		
		this->m_hasNormals = other.m_hasNormals;
		other.m_hasNormals = false;
	}
	
	Mesh& Mesh::operator=(Mesh other)
	{
		this->m_vao = other.m_vao;
		other.m_vao = 0;
		
		this->m_vboV = other.m_vboV;
		other.m_vboV = 0;
		
		this->m_vboU = other.m_vboU;
		other.m_vboU = 0;
		
		this->m_vboN = other.m_vboN;
		other.m_vboN = 0;
		
		this->m_vboI = other.m_vboI;
		other.m_vboI = 0;
		
		this->m_numVerts = other.m_numVerts;
		other.m_numVerts = 0;
		
		this->m_hasVerts = other.m_hasVerts;
		other.m_hasVerts = false;
		
		this->m_hasUVs = other.m_hasUVs;
		other.m_hasUVs = false;
		
		this->m_hasNormals = other.m_hasNormals;
		other.m_hasNormals = false;
		return *this;
	}
	
	Mesh::Mesh(Mesh &&other) noexcept
	{
		this->m_vao = other.m_vao;
		other.m_vao = 0;
		
		this->m_vboV = other.m_vboV;
		other.m_vboV = 0;
		
		this->m_vboU = other.m_vboU;
		other.m_vboU = 0;
		
		this->m_vboN = other.m_vboN;
		other.m_vboN = 0;
		
		this->m_vboI = other.m_vboI;
		other.m_vboI = 0;
		
		this->m_numVerts = other.m_numVerts;
		other.m_numVerts = 0;
		
		this->m_hasVerts = other.m_hasVerts;
		other.m_hasVerts = false;
		
		this->m_hasUVs = other.m_hasUVs;
		other.m_hasUVs = false;
		
		this->m_hasNormals = other.m_hasNormals;
		other.m_hasNormals = false;
	}
	
	Mesh& Mesh::operator=(Mesh &&other) noexcept
	{
		this->m_vao = other.m_vao;
		other.m_vao = 0;
		
		this->m_vboV = other.m_vboV;
		other.m_vboV = 0;
		
		this->m_vboU = other.m_vboU;
		other.m_vboU = 0;
		
		this->m_vboN = other.m_vboN;
		other.m_vboN = 0;
		
		this->m_vboI = other.m_vboI;
		other.m_vboI = 0;
		
		this->m_numVerts = other.m_numVerts;
		other.m_numVerts = 0;
		
		this->m_hasVerts = other.m_hasVerts;
		other.m_hasVerts = false;
		
		this->m_hasUVs = other.m_hasUVs;
		other.m_hasUVs = false;
		
		this->m_hasNormals = other.m_hasNormals;
		other.m_hasNormals = false;
		return *this;
	}
	
	void Mesh::use() const
	{
		glBindVertexArray(this->m_vao);
	}
}
