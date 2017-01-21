#pragma once

#include <map>
#include <glm/glm.hpp>

class IDevice;
class CMesh;
class CShaderProgram;
class CTexture;

class CFontVertex {
public:
  glm::vec2 mPos;
  glm::vec2 mCoord;
};

class CFont {
private:
  struct CharDesc {
    glm::vec2 min;
    glm::vec2 max;
    glm::vec2 tmin;
    glm::vec2 tmax;
    glm::vec2 adv;
  };
  typedef std::map<glm::uint16, CharDesc> fontcharmap_t;

  fontcharmap_t mFontMap;
  CShaderProgram* mProgram;
  CTexture* mTexture;
  glm::int32 mUniTransform;
  glm::int32 mUniColor;
  glm::int32 mUniTexture;

public:
  CFont(IDevice* pDevice);
  ~CFont();

  const bool SetVerts(const glm::uint16 code, glm::vec2& pos, CFontVertex* pVert);
  void SetTransform(const glm::mat4& transform);
  void SetColor(const glm::vec4& color);

  CShaderProgram* GetProgram() const;
  CTexture* GetTexture() const;

  void Bind() const;
  void Unbind() const;
};

class CFontText {
private:
  CFont* mFont;
  CMesh* mMesh;
  std::wstring mText;

public:
  CFontText(CFont* pFont);
  ~CFontText();

  void SetText(const std::wstring& text);

  void Render(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
};
