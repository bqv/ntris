/**
 * \file main.c
 * \brief This file is the main unit of the application.
 * Copyright (C) 2008 Simon Laszlo
 * Copyright (C) 2013 Tony Olagbaiye
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <string.h>

#include <locale.h>

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "m3d.h"
#include "m4d.h"
#include "eng.h"
#include "ai.h"
#include "scn.h"
#include "g3d.h"
#include "g4d.h"
#include "gtxt.h"
#include "ui.h"
#include "menu.h"
#include "hst.h"
#include "conf.h"
#include "mou.h"

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
    mouResize(w, h);
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
        if (strcmp (argv[i], "--debug") == 0)
        {
            debugmode = 1;
        }
    }
}

/** Close tasks */
static void terminate(void)
{
    TTF_Quit();
    SDL_Quit();

    confSave(confUserFilename("ntris"));

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
    SDLMod mods;
    int uiKey;
    int w, h, ok, temp;

    tEngGame engGame, engGameDraw;
    tScnSet scnSet = scnGetDefaultSet(), scnSetDraw;

    processARGV(argc, argv);

    confLoad(confUserFilename("ntris"));

    /* Initialize localization */
    setlocale(LC_ALL, "");

    /*  Initialize the game engine. */
    engInitGame(&engGame, &onGameOver);
    {
        temp = confGetVar("spaceLength", &ok);
        if (ok)
        {
            engGame.spaceLength = temp;
        }
        temp = confGetVar("size_X", &ok);
        if (ok)
        {
            engGame.size[0] = temp;
        }
        temp = confGetVar("size_Y", &ok);
        if (ok)
        {
            engGame.size[1] = temp;
        }
        temp = confGetVar("size_Z", &ok);
        if (ok)
        {
            engGame.size[2] = temp;
        }
        temp = confGetVar("animation_enable", &ok);
        if (ok)
        {
            engGame.animation.enable = temp;
        }
        temp = confGetVar("game_opts_diff", &ok);
        if (ok)
        {
            engGame.game_opts.diff = temp;
        }

        engResetGame(&engGame);
    }

    /*  Initialize/load High Score table */
    hstInit();

    /*  Set random colors for game levels */
    scnInit();
    {
        temp = confGetVar("enableHypercubeDraw", &ok);
        if (ok)
        {
            scnSet.enableHypercubeDraw = temp;
        }
        temp = confGetVar("enableSeparateBlockDraw", &ok);
        if (ok)
        {
            scnSet.enableSeparateBlockDraw = temp;
        }
        temp = confGetVar("enableGridDraw", &ok);
        if (ok)
        {
            scnSet.enableGridDraw = temp;
        }
        temp = confGetVar("viewMode", &ok);
        if (ok)
        {
            scnSet.viewMode = temp;
        }
    }

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    SDL_EnableKeyRepeat(100,SDL_DEFAULT_REPEAT_INTERVAL);

    w = confGetVar("WindowWidth", &ok);
    if (!ok)
    {
        w = 640;
    }
    h = confGetVar("WindowHeight", &ok);
    if (!ok)
    {
        h = 480;
    }

    screen = SDL_SetVideoMode(w, h, 16, SDL_OPENGL|SDL_RESIZABLE);
    if (!screen)
    {
        fprintf(stderr, "Couldn't set GL video mode: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }
    SDL_WM_SetCaption("NTris", "ntris");

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
        int delay;

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
                mods = SDL_GetModState();
                uiKey = 0;
                if(keys[SDLK_UP])
                {
                    uiKey = UI_KEY_UP;
                }
                if(keys[SDLK_LEFT])
                {
                    uiKey = UI_KEY_LEFT;
                }
                if(keys[SDLK_DOWN])
                {
                    uiKey = UI_KEY_DOWN;
                }
                if(keys[SDLK_RIGHT])
                {
                    uiKey = UI_KEY_RIGHT;
                }
                if(keys[SDLK_PAGEUP])
                {
                    uiKey = UI_KEY_PGUP;
                }
                if(keys[SDLK_PAGEDOWN])
                {
                    uiKey = UI_KEY_PGDOWN;
                }
                if(keys[SDLK_HOME])
                {
                    uiKey = UI_KEY_HOME;
                }
                if(keys[SDLK_END])
                {
                    uiKey = UI_KEY_END;
                }
                if(keys[SDLK_F1])
                {
                    uiKey = UI_KEY_F1;
                }
                if(keys[SDLK_F2])
                {
                    uiKey = UI_KEY_F2;
                }
                if(keys[SDLK_ESCAPE])
                {
                    uiKey = UI_KEY_ESC;
                }
                if(keys[SDLK_BACKSPACE])
                {
                    uiKey = UI_KEY_BACKSPACE;
                }
                if(keys[SDLK_TAB])
                {
                    uiKey = UI_KEY_TAB;
                }
                if(keys[SDLK_RETURN])
                {
                    uiKey = UI_KEY_ENTER;
                }
                if(keys[SDLK_DELETE])
                {
                    uiKey = UI_KEY_DEL;
                }
                if(keys[SDLK_INSERT])
                {
                    uiKey = UI_KEY_INS;
                }
                if(keys[SDLK_SPACE])
                {
                    uiKey = ' ';
                }
                if(keys[SDLK_q])
                {
                    uiKey = 'q';
                }
                if(keys[SDLK_w])
                {
                    uiKey = 'w';
                }
                if((mods & KMOD_SHIFT) || (mods & KMOD_CAPS))
                {
                    if(keys[SDLK_a])
                    {
                        uiKey = 'A';
                    }
                    if(keys[SDLK_s])
                    {
                        uiKey = 'S';
                    }
                    if(keys[SDLK_d])
                    {
                        uiKey = 'D';
                    }
                    if(keys[SDLK_f])
                    {
                        uiKey = 'F';
                    }
                    if(keys[SDLK_g])
                    {
                        uiKey = 'G';
                    }
                    if(keys[SDLK_h])
                    {
                        uiKey = 'H';
                    }
                    if(keys[SDLK_z])
                    {
                        uiKey = 'Z';
                    }
                    if(keys[SDLK_x])
                    {
                        uiKey = 'X';
                    }
                    if(keys[SDLK_c])
                    {
                        uiKey = 'C';
                    }
                    if(keys[SDLK_v])
                    {
                        uiKey = 'V';
                    }
                    if(keys[SDLK_b])
                    {
                        uiKey = 'B';
                    }
                    if(keys[SDLK_n])
                    {
                        uiKey = 'N';
                    }
                }
                else
                {
                    if(keys[SDLK_a])
                    {
                        uiKey = 'a';
                    }
                    if(keys[SDLK_s])
                    {
                        uiKey = 's';
                    }
                    if(keys[SDLK_d])
                    {
                        uiKey = 'd';
                    }
                    if(keys[SDLK_f])
                    {
                        uiKey = 'f';
                    }
                    if(keys[SDLK_g])
                    {
                        uiKey = 'g';
                    }
                    if(keys[SDLK_h])
                    {
                        uiKey = 'h';
                    }
                    if(keys[SDLK_z])
                    {
                        uiKey = 'z';
                    }
                    if(keys[SDLK_x])
                    {
                        uiKey = 'x';
                    }
                    if(keys[SDLK_c])
                    {
                        uiKey = 'c';
                    }
                    if(keys[SDLK_v])
                    {
                        uiKey = 'v';
                    }
                    if(keys[SDLK_b])
                    {
                        uiKey = 'b';
                    }
                    if(keys[SDLK_n])
                    {
                        uiKey = 'n';
                    }
                }
                if(keys[SDLK_1])
                {
                    uiKey = '1';
                }
                if(keys[SDLK_2])
                {
                    uiKey = '2';
                }
                if(keys[SDLK_3])
                {
                    uiKey = '3';
                }
                if(uiKey != 0)
                {
                    uiKeyPress(uiKey, &engGame, &scnSet);
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                mouMove(event.motion.x, event.motion.y, &engGame);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                mouDown(event.button.x, event.button.y);
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                mouUp(event.button.x, event.button.y, &engGame);
                break;
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
