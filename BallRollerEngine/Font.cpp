#include "stdafx.h"
#include "Font.h"
#include "calibri.hpp"
#include "Mesh.h"
#include "VertexDefinition.h"
#include "Shader.h"
#include "Device.h"
#include "Texture.h"

#include <glm/gtc/type_ptr.hpp>

CFont::CFont(IDevice* pDevice) {
  for(size_t i = 0; i < g_fontListSize; i++) {
    FontCharDesc& d = g_fontList[i];

    mFontMap[d.code] = {
      glm::vec2(d.minx, d.miny),
      glm::vec2(d.maxx, d.maxy),
      glm::vec2(d.txmin, d.tymin),
      glm::vec2(d.txmax, d.tymax),
      glm::vec2(d.advx, d.advy)
    };
  }

  {
    std::vector<glm::uint8> data;
    pDevice->LoadAsset("calibri.png", data);

    mTexture = new CTexture(GL_RGBA4);
    mTexture->LoadPng(data);
  }

  {
    const std::string vertSource = SHADER_SOURCE(
      precision mediump float;
      uniform mat4 mTransform;
      attribute vec2 vInPos;
      attribute vec2 vInCoord;
      varying vec2 vPassCoord;
      void main() {
        gl_Position = mTransform * vec4(vInPos, 0.0, 1.0);
        vPassCoord = vInCoord;
      }
    );

    const std::string fragSource = SHADER_SOURCE(
      precision mediump float;
      uniform sampler2D texBase;
      uniform vec4 vColor;
      varying vec2 vPassCoord;
      void main() {
        vec4 color = texture2D(texBase, vPassCoord);
        gl_FragColor = color * vColor;
      }
    );

    std::string log;
    mProgram = CreateShaderProgram(vertSource, fragSource, log);
    if(!mProgram) {
      pDevice->Log(log);
    }
  }

  mUniTransform = mProgram->GetUniformIndex("mTransform");
  mUniColor = mProgram->GetUniformIndex("vColor");
  mUniTexture = mProgram->GetUniformIndex("texBase");
}

CFont::~CFont() {
  delete mTexture;
  delete mProgram;
}

const bool CFont::SetVerts(const glm::uint16 code, glm::vec2& pos, CFontVertex * pVert) {
  fontcharmap_t::iterator it = mFontMap.find(code);
  if(it == mFontMap.end())
    return false;

  CharDesc& d = it->second;

  pVert[0].mPos = pos + d.min;
  pVert[1].mPos = pos + glm::vec2(d.max.x, d.min.y);
  pVert[2].mPos = pos + d.max;
  pVert[3].mPos = pos + glm::vec2(d.min.x, d.max.y);

  pVert[0].mCoord = glm::vec2(d.tmin.x, d.tmax.y);
  pVert[1].mCoord = d.tmax;
  pVert[2].mCoord = glm::vec2(d.tmax.x, d.tmin.y);
  pVert[3].mCoord = d.tmin;

  pos += d.adv;

  return true;
}

void CFont::SetTransform(const glm::mat4 & transform) {
  glUniformMatrix4fv(mUniTransform, 1, GL_FALSE, glm::value_ptr(transform));
}

void CFont::SetColor(const glm::vec4 & color) {
  glUniform4fv(mUniColor, 1, glm::value_ptr(color));
}

CShaderProgram * CFont::GetProgram() const {
  return mProgram;
}

CTexture * CFont::GetTexture() const {
  return mTexture;
}

void CFont::Bind() const {
  mProgram->Bind();
  mTexture->Bind();
  glUniform1i(mUniTexture, 0);
}

void CFont::Unbind() const {
  mTexture->Unbind();
  mProgram->Unbind();
}

CFontText::CFontText(CFont* pFont) 
  : mFont(pFont)
  , mMesh(nullptr) 
{
  glm::int32 locVert = pFont->GetProgram()->GetAttributeIndex("vInPos");
  glm::int32 locCoord = pFont->GetProgram()->GetAttributeIndex("vInCoord");

  CVertexDefinition def(sizeof(CFontVertex));
  def.AddStream(locVert, 2, GL_FLOAT, 0);
  def.AddStream(locCoord, 2, GL_FLOAT, 8);

  mMesh = new CMesh(def, GL_TRIANGLES);
}

CFontText::~CFontText() {
  delete mMesh;
}

void CFontText::SetText(const std::wstring & text) {
  if(mText == text || text.empty())
    return;

  std::vector<CFontVertex> vert(text.length() * 4);
  std::vector<GLushort> indx(text.length() * 6);

  glm::vec2 tpos = glm::vec2(0.0f);
  for(size_t i = 0; i < text.length(); i++) {
    glm::uint16 vidx = (glm::uint16)i * 4;
    glm::uint16 iidx = (glm::uint16)i * 6;

    CFontVertex* pVert = &vert[vidx];
    GLushort* pInd = &indx[iidx];

    mFont->SetVerts(text[i], tpos, pVert);

    pInd[0] = vidx + 0;
    pInd[1] = vidx + 1;
    pInd[2] = vidx + 2;

    pInd[3] = vidx + 0;
    pInd[4] = vidx + 2;
    pInd[5] = vidx + 3;
  }

  mMesh->setVertices(vert);
  mMesh->setIndices(indx);
  mText = text;
}

void CFontText::Render(const glm::mat4 & transform, const glm::vec4 & color) {
  if(mText.empty())
    return;

  CBind<CMesh> mesh(mMesh);

  mFont->SetTransform(transform);
  mFont->SetColor(color);
  mMesh->Render();
}
