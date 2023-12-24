#include "glrMesh.hh"

#include <glad/gl.hh>

namespace glr
{
  Mesh::~Mesh()
  {
    glDeleteBuffers(1, &this->vboV);
    glDeleteBuffers(1, &this->vboU);
    glDeleteBuffers(1, &this->vboN);
    glDeleteBuffers(1, &this->vboI);
    glDeleteVertexArrays(1, &this->vao);
  }
  
  Mesh::Mesh(float const *verts, size_t vertsSize)
  {
    if(vertsSize != 0)
    {
      this->hasVerts = true;
      this->numVerts = vertsSize / 3;
    }
    glCreateVertexArrays(1, &this->vao);
    glCreateBuffers(1, &this->vboV);
    
    glNamedBufferData(this->vboV, (GLsizeiptr) (vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayVertexBuffer(this->vao, 0, this->vboV, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vao, 0);
    glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    this->init = true;
  }
  
  Mesh::Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize)
  {
    if(vertsSize != 0)
    {
      this->hasVerts = true;
      this->numVerts = vertsSize / 3;
    }
    if(uvsSize != 0) this->hasUVs = true;
    glCreateVertexArrays(1, &this->vao);
    glCreateBuffers(1, &this->vboV);
    glCreateBuffers(1, &this->vboU);
    
    glNamedBufferData(this->vboV, (GLsizeiptr) (vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayVertexBuffer(this->vao, 0, this->vboV, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vao, 0);
    glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    
    glNamedBufferData(this->vboU, (GLsizeiptr) (uvsSize * sizeof(float)), uvs, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 1, 1);
    glVertexArrayVertexBuffer(this->vao, 1, this->vboU, 0, this->uvStride);
    glEnableVertexArrayAttrib(this->vao, 1);
    glVertexArrayAttribFormat(this->vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
    this->init = true;
  }
  
  Mesh::Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize, float const *normals, size_t normalsSize)
  {
    if(vertsSize != 0)
    {
      this->hasVerts = true;
      this->numVerts = vertsSize / 3;
    }
    if(uvsSize != 0) this->hasUVs = true;
    if(normalsSize != 0) this->hasNormals = true;
    glCreateVertexArrays(1, &this->vao);
    glCreateBuffers(1, &this->vboV);
    glCreateBuffers(1, &this->vboU);
    glCreateBuffers(1, &this->vboN);
    
    glNamedBufferData(this->vboV, (GLsizeiptr) (vertsSize * sizeof(float)), verts, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayVertexBuffer(this->vao, 0, this->vboV, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vao, 0);
    glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    
    glNamedBufferData(this->vboU, (GLsizeiptr) (uvsSize * sizeof(float)), uvs, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 1, 1);
    glVertexArrayVertexBuffer(this->vao, 1, this->vboU, 0, this->uvStride);
    glEnableVertexArrayAttrib(this->vao, 1);
    glVertexArrayAttribFormat(this->vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
    
    glNamedBufferData(this->vboN, (GLsizeiptr) (normalsSize * sizeof(float)), normals, GL_STATIC_DRAW);
    glVertexArrayAttribBinding(this->vao, 2, 2);
    glVertexArrayVertexBuffer(this->vao, 2, this->vboN, 0, this->normalStride);
    glEnableVertexArrayAttrib(this->vao, 2);
    glVertexArrayAttribFormat(this->vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
    this->init = true;
  }
  
  Mesh::Mesh(std::vector<float> const &verts) : Mesh(verts.data(), verts.size())
  {}
  Mesh::Mesh(std::vector<float> const &verts, std::vector<float> const &uvs) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size())
  {}
  Mesh::Mesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size())
  {}
  Mesh::Mesh(std::initializer_list<float> const &verts) : Mesh(verts.begin(), verts.size())
  {}
  Mesh::Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs) : Mesh(verts.begin(), verts.size(), uvs.begin(), uvs.size())
  {}
  Mesh::Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs, std::initializer_list<float> const &normals) : Mesh(verts.begin(), verts.size(), uvs.begin(), uvs.size(), normals.begin(), normals.size())
  {}
  
  Mesh::Mesh(std::vector<uint8_t> const &verts) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4)
  {}
  Mesh::Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4, reinterpret_cast<float const *>(uvs.data()), uvs.size() / 4)
  {}
  Mesh::Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs, std::vector<uint8_t> const &normals) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4, reinterpret_cast<float const *>(uvs.data()), uvs.size() / 4, reinterpret_cast<float const *>(normals.data()), normals.size() / 4)
  {}
  Mesh::Mesh(std::initializer_list<uint8_t> const &verts) : Mesh(reinterpret_cast<float const *>(verts.begin()), verts.size() / 4)
  {}
  Mesh::Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs) : Mesh(reinterpret_cast<float const *>(verts.begin()), verts.size() / 4, reinterpret_cast<float const *>(uvs.begin()), uvs.size() / 4)
  {}
  Mesh::Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs, std::initializer_list<uint8_t> const &normals) : Mesh(reinterpret_cast<float const *>(verts.begin()), verts.size() / 4, reinterpret_cast<float const *>(uvs.begin()), uvs.size() / 4, reinterpret_cast<float const *>(normals.begin()), normals.size() / 4)
  {}
  
  Mesh::Mesh(Mesh &&moveFrom) noexcept
  {
    this->vboV = moveFrom.vboV;
    moveFrom.vboV = std::numeric_limits<uint32_t>::max();
    
    this->vboU = moveFrom.vboU;
    moveFrom.vboU = std::numeric_limits<uint32_t>::max();
    
    this->vboN = moveFrom.vboN;
    moveFrom.vboN = std::numeric_limits<uint32_t>::max();
    
    this->vboI = moveFrom.vboI;
    moveFrom.vboI = std::numeric_limits<uint32_t>::max();
    
    this->vao = moveFrom.vao;
    moveFrom.vao = std::numeric_limits<uint32_t>::max();
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
    this->hasVerts = moveFrom.hasVerts;
    moveFrom.hasVerts = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;
    
    this->init = true;
    moveFrom.init = false;
  }
  
  Mesh& Mesh::operator=(Mesh &&moveFrom) noexcept
  {
    this->vboV = moveFrom.vboV;
    moveFrom.vboV = std::numeric_limits<uint32_t>::max();
    
    this->vboU = moveFrom.vboU;
    moveFrom.vboU = std::numeric_limits<uint32_t>::max();
    
    this->vboN = moveFrom.vboN;
    moveFrom.vboN = std::numeric_limits<uint32_t>::max();
    
    this->vboI = moveFrom.vboI;
    moveFrom.vboI = std::numeric_limits<uint32_t>::max();
    
    this->vao = moveFrom.vao;
    moveFrom.vao = std::numeric_limits<uint32_t>::max();
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
    this->hasVerts = moveFrom.hasVerts;
    moveFrom.hasVerts = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;
    
    this->init = true;
    moveFrom.init = false;
    
    return *this;
  }
  
  bool Mesh::exists() const
  {
    return this->init;
  }
  
  void Mesh::reset()
  {
    glDeleteBuffers(1, &this->vboV);
    glDeleteBuffers(1, &this->vboU);
    glDeleteBuffers(1, &this->vboN);
    glDeleteBuffers(1, &this->vboI);
    glDeleteVertexArrays(1, &this->vao);
    this->vboV = std::numeric_limits<uint32_t>::max();
    this->vboU = std::numeric_limits<uint32_t>::max();
    this->vboN = std::numeric_limits<uint32_t>::max();
    this->vboI = std::numeric_limits<uint32_t>::max();
    this->vao = std::numeric_limits<uint32_t>::max();
    this->numVerts = 0;
    this->hasVerts = false;
    this->hasUVs = false;
    this->hasNormals = false;
    this->init = false;
  }
  
  void Mesh::use() const
  {
    glBindVertexArray(this->vao);
  }
}
