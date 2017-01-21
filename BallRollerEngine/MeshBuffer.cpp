#include "stdafx.h"
#include "MeshBuffer.h"

CMeshBufferBase::CMeshBufferBase() {}

CMeshBufferBase::~CMeshBufferBase() {}

const std::vector<glm::uint16>& CMeshBufferBase::GetIndices() const {
  return mIndices;
}

void CMeshBufferBase::AddPoint(const glm::uint16 idx) {
  mIndices.push_back(idx);
}

void CMeshBufferBase::AddLine(const glm::uint16 idx1, const glm::uint16 idx2) {
  mIndices.push_back(idx1);
  mIndices.push_back(idx2);
}

void CMeshBufferBase::AddTriangle(const glm::uint16 idx1, const glm::uint16 idx2, const glm::uint16 idx3) {
  mIndices.push_back(idx1);
  mIndices.push_back(idx2);
  mIndices.push_back(idx3);
}
