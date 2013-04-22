/**
 * \file  cptr.cpp
 * \brief Screenshot capture module
 */

#include <GL/gl.h>
#include <SDL/SDL.h>

int SDL_OGL_SaveScreenshot(const char *FileName,
                           int width,
                           int height)
{
    unsigned char *pixels = NULL;
    unsigned char *pixbuf = NULL;
    SDL_Surface   *screen = NULL;
    int i;

    pixels = malloc(width * height * 4);
    pixbuf = malloc(width * height * 4);

    screen = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                  width, height, 32,
                                  0x0000ff, 0x00ff00, 0xff0000, 0x000000);

    if(    (pixels != NULL)
            && (pixbuf != NULL)
            && (screen != NULL)
      )
    {
        glReadPixels(0, 0,
                     width, height,
                     GL_BGRA, GL_UNSIGNED_BYTE,
                     pixbuf);

        for(i=0; i < height; ++i)
            memcpy(pixels+(height-i-1)*width*4, pixbuf+i*width*4, width*4);

        screen->pixels = pixels;
        SDL_SaveBMP(screen, FileName);
    }
    else
    {
        fprintf(stderr, "Capturing screenshot failed.");
    }

    if (pixels != NULL) free(pixels);
    if (pixbuf != NULL) free(pixbuf);
    if (screen != NULL) SDL_FreeSurface(screen);

    return 1;
}
