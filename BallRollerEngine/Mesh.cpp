#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(const CVertexDefinition & vertexDef, const glm::uint32 polyType)
  : mVertexBuffer(GL_ARRAY_BUFFER)
  , mIndexBuffer(GL_ELEMENT_ARRAY_BUFFER)
  , mVertexDef(vertexDef)
  , mPolyType(polyType)
  , mNumberOfElements(0) 
{}

CMesh::~CMesh() {}

void CMesh::setIndices(const std::vector<glm::uint16>& indices) {
  mIndexBuffer.Load(indices, true);
  mNumberOfElements = indices.size();
}

const glm::uint32 CMesh::GetPolyType() const {
  return mPolyType;
}

const glm::uint32 CMesh::GetNumberOfElements() const {
  return mNumberOfElements;
}

void CMesh::Bind() const {
  mVertexBuffer.Bind();
  mVertexDef.Bind();
  mIndexBuffer.Bind();
}

void CMesh::Unbind() const {
  mIndexBuffer.Unbind();
  mVertexDef.Unbind();
  mVertexBuffer.Unbind();
}

void CMesh::Render() const {
  Render(mPolyType, mNumberOfElements, 0);
}

void CMesh::Render(const glm::uint32 polyType, 
                   const glm::uint32 numberOfElements, 
                   const glm::uint32 elementOffset) const {
  glDrawElements(polyType, 
                 numberOfElements, 
                 GL_UNSIGNED_SHORT, 
                 reinterpret_cast<void*>(elementOffset * sizeof(glm::uint16)));
}
