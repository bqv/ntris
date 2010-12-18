/**
   \mainpage 4DTRIS - Tetris like game in four dimension
   \image html 4DTRIS_icon_128.png

   \section License License

   Copyright (C) 2008 Simon, Laszlo

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   \warning You should have received a copy of the GNU General Public License
            along with this program.  If not, see <http://www.gnu.org/licenses/>.

   \author Simon, Laszlo <laszlo.simon@gmail.com>

   \section Source_diagram Source diagram

   \dotfile 4dtris-sw-struct.dot

   \section Read_me Read me

   \include README
 */
/**
 * \file 4dt_main.c
 * \brief This file is the main unit of the application.
 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <string.h>

#include <libintl.h>
#include <locale.h>

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_ai.h"
#include "4dt_scn.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_gtxt.h"
#include "4dt_ui.h"
#include "4dt_menu.h"
#include "4dt_hst.h"
#include "4dt_conf.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

static int debugmode = 0;

static const int framerate = 50;

static SDL_Surface *screen;

/*
--------------------------------------------------------------------------------
   PROTOTYPES
--------------------------------------------------------------------------------
*/

static void processARGV(int argc, char *argv[]);
static void onGameOver(tEngGame *pEngGame);
static void terminate(void);
static void resize(int w, int h);

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
    Event handlers
*/

/** tasks on resize */
static void resize(int w, int h)
{
  g3dResize(w, h);
  gtxtResize(w, h);
  confSetVar("WindowWidth", (double)w);
  confSetVar("WindowHeight", (double)h);
}

/** Event handler of engine game over */
static void onGameOver(tEngGame *pEngGame)
{
  aiSetActive(0, pEngGame);

  hstAddScore(pEngGame->score);

  menuGotoItem(eMenuGameOver);
}

/** Process command line arguments */
static void processARGV(int argc, char *argv[])
{
  int i;

  for (i = 0; i < argc; i++)
  {
    if (strcmp (argv[i], "--debug") == 0) { debugmode = 1; }
  }
}

/** Close tasks */
static void terminate(void)
{
  TTF_Quit();
  SDL_Quit();

  confSave(confUserFilename("4dtris"));

  exit(0);
}

/*------------------------------------------------------------------------------
    M A I N
*/

/** Main function of the software */
int main(int argc, char *argv[])
{
  int done;
  Uint8 *keys;
  int uiKey;
  int w, h, ok, temp;

  tEngGame engGame, engGameDraw;
  tScnSet scnSet = scnGetDefaultSet(), scnSetDraw;

  processARGV(argc, argv);

  confLoad(confUserFilename("4dtris"));
  printf("Loaded\n");

  /* Initialize localization */
  setlocale(LC_ALL, "");
  textdomain("4dtris");

  /*  Initialize the game engine. */
  engInitGame(&engGame, &onGameOver);
  {
    temp = confGetVar("spaceLength", &ok);
    if (ok) { engGame.spaceLength = temp; }
    temp = confGetVar("size_X", &ok);
    if (ok) { engGame.size[0] = temp; }
    temp = confGetVar("size_Y", &ok);
    if (ok) { engGame.size[1] = temp; }
    temp = confGetVar("size_Z", &ok);
    if (ok) { engGame.size[2] = temp; }
    temp = confGetVar("animation_enable", &ok);
    if (ok) { engGame.animation.enable = temp; }
    temp = confGetVar("game_opts_diff", &ok);
    if (ok) { engGame.game_opts.diff = temp; }

    engResetGame(&engGame);
  }

  /*  Initialize/load High Score table */
  hstInit();

  /*  Set random colors for game levels */
  scnInit();
  {
    temp = confGetVar("enableHypercubeDraw", &ok);
    if (ok) { scnSet.enableHypercubeDraw = temp; }
    temp = confGetVar("enableSeparateBlockDraw", &ok);
    if (ok) { scnSet.enableSeparateBlockDraw = temp; }
    temp = confGetVar("enableGridDraw", &ok);
    if (ok) { scnSet.enableGridDraw = temp; }
    temp = confGetVar("viewMode", &ok);
    if (ok) { scnSet.viewMode = temp; }
  }

  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
  SDL_EnableKeyRepeat(100,SDL_DEFAULT_REPEAT_INTERVAL);

  w = confGetVar("WindowWidth", &ok); if (!ok) { w = 640; }
  h = confGetVar("WindowHeight", &ok); if (!ok) { h = 480; }

  screen = SDL_SetVideoMode(w, h, 16, SDL_OPENGL|SDL_RESIZABLE);
  if (!screen)
  {
    fprintf(stderr, "Couldn't set GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("4DTris", "4dtris");

  if (TTF_Init() != 0)
  {
    fprintf(stderr, "Couldn't initialise SDL_ttf!\n");
    SDL_Quit();
    exit(3);
  }

  /*  initialise 3D drawing modul */
  g3dInit();

  /*  initialise 4D drawing modul */
  g4dInit(engGame.spaceLength);
  g4dSwitchAutoRotation(1);

  /* Initialize menu */
  menuInit(&engGame, &scnSet);
  menuSetOnActivate(eMenuQuit, &terminate);

  /*  start autoplayer */
  aiSetActive(1, &engGame);

  resize(screen->w, screen->h);
  done = 0;

  while ( ! done )
  {
    SDL_Event event;
    Uint32 startFrame;
    Uint32 endFrame;
    Uint32 delay;

    startFrame = SDL_GetTicks();

    while ( SDL_PollEvent(&event) )
    {
      switch(event.type)
      {
        case SDL_VIDEORESIZE:
        {
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                    SDL_OPENGL|SDL_RESIZABLE);
          if (screen)
          {
            resize(screen->w, screen->h);
          }
          break;
        }
        case SDL_QUIT:
        {
          done = 1;
          break;
        }
        case SDL_KEYDOWN:
        {
          keys = SDL_GetKeyState(NULL);
          uiKey = 0;
          if(keys[SDLK_UP])        { uiKey = UI_KEY_UP;       }
          if(keys[SDLK_LEFT])      { uiKey = UI_KEY_LEFT;     }
          if(keys[SDLK_DOWN])      { uiKey = UI_KEY_DOWN;     }
          if(keys[SDLK_RIGHT])     { uiKey = UI_KEY_RIGHT;    }
          if(keys[SDLK_PAGEUP])    { uiKey = UI_KEY_PGUP;     }
          if(keys[SDLK_PAGEDOWN])  { uiKey = UI_KEY_PGDOWN;   }
          if(keys[SDLK_HOME])      { uiKey = UI_KEY_HOME;     }
          if(keys[SDLK_END])       { uiKey = UI_KEY_END;      }
          if(keys[SDLK_F1])        { uiKey = UI_KEY_F1;       }
          if(keys[SDLK_F2])        { uiKey = UI_KEY_F2;       }
          if(keys[SDLK_ESCAPE])    { uiKey = UI_KEY_ESC;      }
          if(keys[SDLK_BACKSPACE]) { uiKey = UI_KEY_BACKSPACE;}
          if(keys[SDLK_TAB])       { uiKey = UI_KEY_TAB;      }
          if(keys[SDLK_RETURN])    { uiKey = UI_KEY_ENTER;    }
          if(keys[SDLK_DELETE])    { uiKey = UI_KEY_DEL;      }
          if(keys[SDLK_INSERT])    { uiKey = UI_KEY_INS;      }
          if(keys[SDLK_SPACE])     { uiKey = ' ';             }
          if(keys[SDLK_a])         { uiKey = 'a';             }
          if(keys[SDLK_z])         { uiKey = 'z';             }
          if(keys[SDLK_x])         { uiKey = 'x';             }
          if(keys[SDLK_c])         { uiKey = 'c';             }
          if(keys[SDLK_v])         { uiKey = 'v';             }
          if(keys[SDLK_1])         { uiKey = '1';             }
          if(keys[SDLK_2])         { uiKey = '2';             }
          if(keys[SDLK_3])         { uiKey = '3';             }
          if(uiKey != 0) { uiKeyPress(uiKey, &engGame, &scnSet); }
        }
      }
    }

    engGameDraw = engGame;
    scnSetDraw  = scnSet;
    scnDisplay(&engGameDraw, &scnSetDraw);

    endFrame = SDL_GetTicks();

    delay = 1000/framerate - (endFrame - startFrame);
    if (delay < 0)
    {
      delay = 0;
    }
    else if (delay > 1000/framerate)
    {
      delay = 1000/framerate;
    }
    SDL_Delay(delay);
  }
  terminate();

  /*  Return with successed exit constant. */
  return 0;
}
