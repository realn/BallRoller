
#include <iostream>
#include <string>
#include <map>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_ttf.h>

std::wstring lowerChars = L"aπbcÊdeÍfghijkl≥mnÒoÛpqursútvwxyzüø";
std::wstring upperChars = L"A•BC∆DE FGHIJKL£MN—O”PQURSåTVWXYZèØ";
std::wstring specialChars = L"0123456789 !,./\\';[]{}-=_+:\"<>?|`~@#$%^&*()";
std::wstring allChars = lowerChars + upperChars + specialChars;

int main(int argc, char* argv[]) {
  if(argc < 2)
    return 0;

  const std::string fontName = argv[1];
  const std::string fontPath = "Fonts/" + fontName + ".ttf";
  const Uint32 fontSize = 16;

  if(SDL_Init(0) != 0) {
    std::cout << "SDL init faild.";
    return -1;
  }
  if(TTF_Init() != 0) {
    std::cout << "SDL_ttf init failed.";
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

  int tx = 0;
  int ty = 0;
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
  }

  SDL_SaveBMP(pTexture, "test.bmp");

  SDL_FreeSurface(pTexture);
  TTF_CloseFont(pFont);

  TTF_Quit();
  SDL_Quit();

  std::cin.get();
  return 0;
}