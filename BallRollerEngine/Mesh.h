#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "VertexDefinition.h"

class CMesh {
private:
  CBuffer mVertexBuffer;
  CBuffer mIndexBuffer;
  CVertexDefinition mVertexDef;
  GLenum mPolyType;
  GLuint mNumberOfElements;

public:
  CMesh(const CVertexDefinition& vertexDef, const GLenum polyType = GL_TRIANGLES);
  ~CMesh();

  void setIndices(const std::vector<GLushort>& indices);
  template<typename _Type>
  void setVertices(const std::vector<_Type>& vertices) {
    this->mVertexBuffer.Load(vertices, true);
  }

  const GLenum GetPolyType() const;
  const GLuint GetNumberOfElements() const;

  void Bind() const;
  void Unbind() const;

  void Render() const;
  void Render(const GLenum polyType, const GLuint numberOfElements, const GLuint elementOffset = 0) const;
};