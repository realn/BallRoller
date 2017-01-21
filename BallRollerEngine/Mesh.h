#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "VertexDefinition.h"

class CMesh {
private:
  CBuffer mVertexBuffer;
  CBuffer mIndexBuffer;
  CVertexDefinition mVertexDef;
  glm::uint32 mPolyType;
  glm::uint32 mNumberOfElements;

public:
  CMesh(const CVertexDefinition& vertexDef, const glm::uint32 polyType = GL_TRIANGLES);
  ~CMesh();

  void setIndices(const std::vector<glm::uint16>& indices);
  template<typename _Type>
  void setVertices(const std::vector<_Type>& vertices) {
    this->mVertexBuffer.Load(vertices, true);
  }

  const glm::uint32 GetPolyType() const;
  const glm::uint32 GetNumberOfElements() const;

  void Bind() const;
  void Unbind() const;

  void Render() const;
  void Render(const glm::uint32 polyType, 
              const glm::uint32 numberOfElements, 
              const glm::uint32 elementOffset = 0) const;
};