/**
 * \file  4dt_gtxt.c
 * \brief Text drawing modul.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <math.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#ifndef __MINGW32__
#include <fontconfig/fontconfig.h>
#endif

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/


static int gtxtWidth;
static int gtxtHeight;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

int nextpoweroftwo(int x)
{
  double logbase2 = log(x) / log(2);
  return((int)pow(2,ceil(logbase2)));
}

void gtxtResize(int width, int height)
{
  gtxtWidth = width;
  gtxtHeight = height;
}

void SDL_GL_RenderText(char *text,
                      TTF_Font *font,
                      SDL_Color color,
                      SDL_Rect *location,
                      float alpha)
{
  SDL_Surface *initial;
  SDL_Surface *intermediary;
  int w,h;
  GLuint texture;

  /* Use SDL_TTF to render our text */
  initial = TTF_RenderUTF8_Blended(font, text, color);

  /* Convert the rendered text to a known format */
  w = nextpoweroftwo(initial->w);
  h = nextpoweroftwo(initial->h);

  intermediary = SDL_CreateRGBSurface(0, w, h, 32,
                  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

  SDL_BlitSurface(initial, 0, intermediary, 0);

  /* Tell GL about our new texture */
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, intermediary->pixels );

  /* GL_NEAREST looks horrible, if scaled... */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* prepare to render our texture */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glColor4f(alpha, alpha, alpha, alpha);

  /* Draw a quad at location */
  glBegin(GL_QUADS);
    /* Recall that the origin is in the lower-left corner
       That is why the TexCoords specify different corners
       than the Vertex coors seem to. */
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(location->x    , location->y);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(location->x + w, location->y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(location->x + w, location->y + h);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(location->x    , location->y + h);
  glEnd();

  /* Bad things happen if we delete the texture before it finishes */
  glFinish();

  /* return the deltas in the unused w,h part of the rect */
  location->w = initial->w;
  location->h = initial->h;

  /* Clean up */
  SDL_FreeSurface(initial);
  SDL_FreeSurface(intermediary);
  glDeleteTextures(1, &texture);
}


/** Draw bitmap string to the specified coordinates and font. */
void g3dRenderString(double x, double y,
                    float color[4],
                    char *string)
{
  const char *font_full_name = "Liberation Sans Bold Italic";
  SDL_Color colorf;
  SDL_Rect position;
  int vPort[4];
#ifndef __MINGW32__
  FcPattern *pat = NULL;
  FcObjectSet *os = NULL;
  FcFontSet *fs = NULL;
#endif

  /* Go in HUD-drawing mode */

  TTF_Font* font;

#ifndef __MINGW32__
  pat = FcPatternBuild (0, FC_FULLNAME, FcTypeString, font_full_name, NULL);
  fs = FcFontList(NULL, pat, os);
  if (fs && (fs->nfont > 0)) {
    char* fontname;
    FcPatternGetString(fs->fonts[0], FC_FILE, 0, (FcChar8 **) &fontname);
    font = TTF_OpenFont(fontname, gtxtHeight / 24);
  } else {
    printf("Unable to find font: %s\n", font_full_name);
    exit(1);
  }
  if (font == NULL)
  {
    printf("Unable to load font: %s \n", TTF_GetError());
    exit(1);
  }
#else
  if (font == NULL)
  {
    font = TTF_OpenFont("/usr/share/fonts/TTF/ttf-liberation/"
        "LiberationSans-BoldItalic.ttf",
        gtxtHeight / 24);
  }
#endif

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glGetIntegerv(GL_VIEWPORT, vPort);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);

  /* Draw some text */
  colorf.r = 255 * color[0];
  colorf.g = 255 * color[1];
  colorf.b = 255 * color[2];
  /** A quick note about position.
   * Enable2D puts the origin in the lower-left corner of the
   * screen, which is different from the normal coordinate
   * space of most 2D api's. position, therefore,
   * gives the X,Y coordinates of the lower-left corner of the
   * rectangle **/
  position.x = gtxtWidth * x;
  position.y = gtxtHeight * y;
  SDL_GL_RenderText(string, font, colorf, &position, color[3]);

  /* Come out of HUD mode */
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();


  TTF_CloseFont(font);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/** Draw bitmap text to the specified coordinates and font. */
void g3dRenderText(double x, double y,
                   float color[4],
                   char **strings,
                   int lineNum,
                   double lineSpace)
{
  int i;

  for (i = 0; i < lineNum; i++)
  {
    if (strings[i][0] != '\0')
    {
      g3dRenderString(x, y - i * lineSpace, color, strings[i]);
    }
  }
}
