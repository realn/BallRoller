
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

std::wstring lowerChars = L"aπbcÊdeÍfghijkl≥mnÒoÛpqursútvwxyzüø";
std::wstring upperChars = L"A•BC∆DE FGHIJKL£MN—O”PQURSåTVWXYZèØ";
std::wstring specialChars = L"0123456789 !,./\\';[]{}-=_+:\"<>?|`~@#$%^&*()";
std::wstring allChars = lowerChars + upperChars + specialChars;

typedef std::vector<std::string> strvec_t;

struct FontCharDesc {
  Uint16 code;
  Uint16 flags;
  float minx;
  float miny;
  float maxx;
  float maxy;
  float advx;
  float advy;
  float txmin;
  float txmax;
  float tymin;
  float tymax;
};

typedef std::vector<FontCharDesc> chardescvec_t;

const strvec_t GetCmdVec(int argc, char* argv[]) {
  strvec_t result;
  for(int i = 0; i < argc; i++)
    result.push_back(argv[i]);
  return result;
}

const std::string FindArgStr(const strvec_t& args, const std::string& argName, const std::string& defValue) {
  strvec_t::const_iterator it = std::find(args.begin(), args.end(), argName);
  if(it == args.end())
    return defValue;

  it++;
  if(it == args.end())
    return defValue;

  return *it;
}

template<typename _Type>
const _Type FindArg(const strvec_t& args, const std::string& argName, const _Type& defValue) {
  std::string res = FindArgStr(args, argName, std::string());
  if(res.empty())
    return defValue;

  std::stringstream ss;
  _Type resVal;
  ss << res;
  ss >> resVal;
  if(ss.fail())
    return defValue;

  return resVal;
}

void printUsage() {
  std::cout << "FontCompiler <FontName> [-s <Size=16>] [-ts <textureSize=256>] [-ot <bin|hpp|xml>] [-o <outFileName>]" << std::endl;
}

int main(int argc, char* argv[]) {
  strvec_t args = GetCmdVec(argc, argv);
  if(args.size() < 2) {
    printUsage();
    return 0;
  }

  const std::string fontName = argv[1];
  const std::string fontPath = "Fonts/" + fontName + ".ttf";
  const Uint32 fontSize = FindArg(args, "-s", 16);
  const Uint32 texSize = FindArg(args, "-ts", 256);
  const std::string outFileName = FindArgStr(args, "-o", fontName);
  const std::string outType = FindArgStr(args, "-ot", "hpp");

  if(SDL_Init(0) != 0) {
    std::cout << "SDL init faild.";
    return -1;
  }
  if(TTF_Init() != 0) {
    std::cout << "SDL_ttf init failed.";
    return -1;
  }
  if(IMG_Init(IMG_INIT_PNG) != 0) {
    std::cout << "SDL_image init failed.";
    return -1;
  }

  //SDL_RWops* pFontFile = SDL_RWFromFile(fontPath.c_str(), "rb");
  TTF_Font* pFont = TTF_OpenFont(fontPath.c_str(), fontSize);
  if(pFont == NULL) {
    std::cout << "SDL font loading failed.";
    return -1;
  }

  SDL_Color bg = { 0, 0, 0, 0 };
  SDL_Color fg = {255, 255, 255, 255};

  SDL_Surface* pTexture = SDL_CreateRGBSurface(0, 256, 256, 32, 
                                               0xFF000000,
                                               0x00FF0000,
                                               0x0000FF00,
                                               0x000000FF);

  chardescvec_t charlist;

  int tx = 0;
  int ty = 0;
  float fsize = (float)fontSize;
  float tsize = (float)texSize;
  for(std::wstring::iterator it = allChars.begin(); it != allChars.end(); it++) {
    int minx, miny, maxx, maxy, adv;

    if(TTF_GlyphMetrics(pFont, (Uint16)(*it), &minx, &maxx, &miny, &maxy, &adv) != 0) {
      std::wcout << L"Failed to process char " << *it << std::endl;
      continue;
    }

    SDL_Surface* pCharGlyph = TTF_RenderGlyph_Shaded(pFont, (Uint16)(*it), fg, bg);
    if(pCharGlyph == nullptr)
      continue;

    if(tx + pCharGlyph->w > 256) {
      ty += fontSize;
      tx = 0;
    }
    if(ty + fontSize > 256)
      continue;

    SDL_Rect target;
    target.x = tx;
    target.y = ty;
    target.w = pCharGlyph->w;
    target.h = pCharGlyph->h;

    if(SDL_BlitSurface(pCharGlyph, NULL, pTexture, &target) != 0) {
      std::wcout << L"Failed to blit char " << *it << std::endl;
      continue;
    }

    tx += pCharGlyph->w;
    SDL_FreeSurface(pCharGlyph);

    FontCharDesc desc = {0};
    desc.code = (Uint16)(*it);
    desc.minx = (float)minx / fsize;
    desc.miny = (float)miny / fsize;
    desc.maxx = (float)maxx / fsize;
    desc.maxy = (float)maxy / fsize;
    desc.advx = (float)adv / fsize;
    desc.advy = 0.0f;
    desc.txmin = (float)target.x / tsize;
    desc.tymin = (float)target.y / tsize;
    desc.txmax = (float)(target.x + target.w) / fsize;
    desc.tymax = (float)(target.y + target.h) / fsize;

    charlist.push_back(desc);
  }

  std::string fontImgFileName = outFileName + ".png";
  if(IMG_SavePNG(pTexture, fontImgFileName.c_str()) != 0) {
    std::cout << "Failed to save texture." << std::endl;
  }

  if(outType == "hpp") {

  }

  SDL_FreeSurface(pTexture);
  TTF_CloseFont(pFont);

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  std::cin.get();
  return 0;
}