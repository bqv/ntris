/*
 * 4dt-cptr.cpp
 *
 *  Created on: 1 Nov 2010
 *      Author: yals
 */

#include <GL/gl.h>
#include <SDL/SDL.h>

#define W 480
#define H 360

unsigned char pixels[W * H * 4];
unsigned char pixelsbuf[W * H * 4];
SDL_Surface *screen = NULL;

int SDL_OGL_SaveScreenshot(const char *FileName)
{
  int i;

  if (screen==NULL)
  {
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                  W, H, 32,
                                  0x0000ff, 0x00ff00, 0xff0000, 0x000000);
  }
  glReadPixels(0, 0, W, H, GL_BGRA, GL_UNSIGNED_BYTE, pixelsbuf);

  for(i=0; i < H; ++i)
     memcpy(pixels+(H-i-1)*W*4, pixelsbuf+i*W*4, W*4);

  screen->pixels = pixels;
  SDL_SaveBMP(screen, FileName);

  return 1;
}
