#include "stdafx.h"
#include "MeshBuffer.h"

CMeshBufferBase::CMeshBufferBase() {}

CMeshBufferBase::~CMeshBufferBase() {}

const std::vector<GLushort>& CMeshBufferBase::GetIndices() const {
  return mIndices;
}

void CMeshBufferBase::AddPoint(const GLushort idx) {
  mIndices.push_back(idx);
}

void CMeshBufferBase::AddLine(const GLushort idx1, const GLushort idx2) {
  mIndices.push_back(idx1);
  mIndices.push_back(idx2);
}

void CMeshBufferBase::AddTriangle(const GLushort idx1, const GLushort idx2, const GLushort idx3) {
  mIndices.push_back(idx1);
  mIndices.push_back(idx2);
  mIndices.push_back(idx3);
}
