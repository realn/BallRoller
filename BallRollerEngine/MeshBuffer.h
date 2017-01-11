#pragma once

#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>

class CMeshBufferBase {
private:
  std::vector<GLushort> mIndices;

public:
  CMeshBufferBase();
  virtual ~CMeshBufferBase();

  const std::vector<GLushort>&  GetIndices() const;

  void AddPoint(const GLushort idx);
  void AddLine(const GLushort idx1, const GLushort idx2);
  void AddTriangle(const GLushort idx1, const GLushort idx2, const GLushort idx3);
};

template<typename _Type>
class CMeshBuffer : public CMeshBufferBase {
private:
  std::vector<_Type> mVertices;

public:
  CMeshBuffer() {}
  virtual ~CMeshBuffer() {}

  const std::vector<_Type>& GetVertices() const;

  void AddVertex(const _Type& vertex) {
    GLushort idx = mVertices.size();
    if(!FindVertex(vertex, idx)) {
      mVertices.push_back(vertex);
    }
    AddPoint(idx);
  }
  void AddLine(const _Type& v1, const _Type& v2) {
    AddVertex(v1);
    AddVertex(v2);
  }
  void AddTriangle(const _Type& v1, const _Type& v2, const _Type& v3) {
    AddVertex(v1);
    AddVertex(v2);
    AddVertex(v3);
  }

private:
  const bool FindVertex(const _Type& vertex, GLushort& outIndex) const {
    typename std::vector<_Type>::const_iterator it = std::find(mVertices.begin(), mVertices.end(), vertex);
    if(it == mVertices.end())
      return false;

    outIndex = (GLushort)(it - mVertices.begin());
    return true;
  }
};