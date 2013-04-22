/**
 * \file  4dt_ui.c
 * \brief Modul of user interface event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>

#include "m.h"
#include "m3d.h"
#include "m4d.h"
#include "eng.h"
#include "ai.h"
#include "scn.h"
#include "g4d.h"
#include "conf.h"
#include "menu.h"
#include "menu.h"
#include "timer.h"

#include "ui.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Angle of rotation on viewport by one keypress [rad] */
static const double uiViewportRotAngle = 15.0 * M_PI / 180.0;

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


/** Eventhandler of special key pressing. */
void uiKeyPress(int key, tEngGame *pEngGame, tScnSet *pScnSet)
{
    /*  if game menu is activated */
    if(menuIsActived())
    {
        /*  Forward user action to menu module */
        /*  Check which key pressed. */
        switch (key)
        {
        case UI_KEY_UP:
            menuNavigate(eMenuUp);
            break;
        case UI_KEY_ESC:
        case UI_KEY_BACKSPACE:
        case UI_KEY_LEFT:
            menuNavigate(eMenuBack);
            break;
        case UI_KEY_TAB:
        case UI_KEY_DOWN:
            menuNavigate(eMenuDown);
            break;
        case UI_KEY_ENTER:
        case ' ':
        case UI_KEY_RIGHT:
            menuNavigate(eMenuForward);
            break;
        }
    }
    else
    {
        /*  Check which key pressed. */
        switch (key)
        {
            /*  If escape */
        case UI_KEY_ESC:
            /*  go to the menu */
            menuNavigate(eMenuForward);
            break;

            /*  If up arrow then */
        case UI_KEY_UP:
            /*  rotate viewport around x axis */
            /*  to positive direction. */
            g4dRotateViewport(eM4dAxisY, eM4dAxisZ, -uiViewportRotAngle);
            break;

            /*  If up arrow then */
        case UI_KEY_LEFT:
            /*  rotate viewport around z axis */
            /*  to negative direction. */
            g4dRotateViewport(eM4dAxisX, eM4dAxisY, -uiViewportRotAngle);
            break;

            /*  If up arrow then */
        case UI_KEY_DOWN:
            /*  rotate viewport around x axis */
            /*  to positive direction. */
            g4dRotateViewport(eM4dAxisY, eM4dAxisZ, uiViewportRotAngle);
            break;

            /*  If up arrow then */
        case UI_KEY_RIGHT:
            /*  rotate viewport around z axis */
            /*  to negative direction. */
            g4dRotateViewport(eM4dAxisX, eM4dAxisY, uiViewportRotAngle);
            break;

            /*  If 'z' then */
            /*  turn the solid */
        case 'z':
            engTurn(1, 2, 1, 1, pEngGame);
            break;
        case 'Z':
            engTurn(1, 2, -1, 1, pEngGame);
            break;
            /*  If 'a' then */
            /*  move the solid */
        case 'a':
            engMove(0, 1, pEngGame);
            break;
        case 'A':
            engMove(0, -1, pEngGame);
            break;

            /*  If 'x' then */
            /*  turn the solid */
        case 'x':
            engTurn(2, 0, 1, 1, pEngGame);
            break;
        case 'X':
            engTurn(2, 0, -1, 1, pEngGame);
            break;
            /*  If 's' then */
            /*  move the solid */
        case 's':
            engMove(1, 1, pEngGame);
            break;
        case 'S':
            engMove(1, -1, pEngGame);
            break;

            /*  If 'c' then */
            /*  turn the solid */
        case 'c':
            engTurn(0, 1, 1, 1, pEngGame);
            break;
        case 'C':
            engTurn(0, 1, -1, 1, pEngGame);
            break;
            /*  If 'd' then */
            /*  move the solid */
        case 'd':
            engMove(2, 1, pEngGame);
            break;
        case 'D':
            engMove(2, -1, pEngGame);
            break;

            /*  If 'v' then */
            /*  turn the solid */
        case 'v':
            engTurn(0, 3, 1, 1, pEngGame);
            break;
        case 'V':
            engTurn(0, 3, -1, 1, pEngGame);
            break;
            /*  If 'f' then */
            /*  rotate the viewport */
        case 'f':
            g4dRotateViewport(eM4dAxisX, eM4dAxisW, uiViewportRotAngle);
            break;
        case 'F':
            g4dRotateViewport(eM4dAxisX, eM4dAxisW, -uiViewportRotAngle);
            break;

            /*  If 'b' then */
            /*  turn the solid */
        case 'b':
            engTurn(1, 3, 1, 1, pEngGame);
            break;
        case 'B':
            engTurn(1, 3, -1, 1, pEngGame);
            break;
            /*  If 'g' then */
            /*  rotate the viewport */
        case 'g':
            g4dRotateViewport(eM4dAxisY, eM4dAxisW, uiViewportRotAngle);
            break;
        case 'G':
            g4dRotateViewport(eM4dAxisY, eM4dAxisW, -uiViewportRotAngle);
            break;

            /*  If 'n' then */
            /*  turn the solid */
        case 'n':
            engTurn(2, 3, 1, 1, pEngGame);
            break;
        case 'N':
            engTurn(2, 3, -1, 1, pEngGame);
            break;
            /*  If 'h' then */
            /*  rotate the viewport */
        case 'h':
            g4dRotateViewport(eM4dAxisZ, eM4dAxisW, uiViewportRotAngle);
            break;
        case 'H':
            g4dRotateViewport(eM4dAxisZ, eM4dAxisW, -uiViewportRotAngle);
            break;

            /*  If space then */
        case ' ':
            /*  step down the solid. */
            engLowerSolid(pEngGame);
            break;

        case '1':
            pScnSet->axle = 0;
            break;

        case '2':
            pScnSet->axle = 1;
            break;

        case '3':
            pScnSet->axle = 2;
            break;

        case UI_KEY_PGUP:
        case '+':
        {
            /*  turn around selected axle clockwise */
            switch(pScnSet->axle)
            {
            case '0':
                engTurn(1, 2, 1, 1, pEngGame);
                break;
            case '1':
                engTurn(0, 2, 1, 1, pEngGame);
                break;
            case '2':
                engTurn(0, 1, 1, 1, pEngGame);
                break;
            case '3':
                engTurn(2, 3, 1, 1, pEngGame);
                break;
            }
            /* engTurn((pScnSet->axle == 0) ? 1 : 0,
                    (pScnSet->axle == 2) ? 1 : 2,
                    1, 1, pEngGame); */
        }
        break;

        case UI_KEY_PGDOWN:
        case '-':
        {
            /*  turn around selected axle counterclockwise */
            engTurn((pScnSet->axle == 2) ? 1 : 2,
                    (pScnSet->axle == 0) ? 1 : 0,
                    1, 1, pEngGame);
        }
        break;

        case UI_KEY_DEL:
            engMove(pScnSet->axle, 1, pEngGame);
            break;

        case UI_KEY_INS:
            engMove(pScnSet->axle, -1, pEngGame);
            break;

        case 'q':
            /*  Quit. */
            TTF_Quit();
            SDL_Quit();
            confSave(confUserFilename("ntris"));
            exit(0);
            break;

        case 'w':
            /*  Switch the autoplayer on. */
            aiSetActive(aiIsActive() ? 0 : 1, pEngGame);
            break;

        case UI_KEY_F1:
            g4dSetViewType(eG4d1PointProjection);
            break;

        case UI_KEY_F2:
            g4dSetViewType(eG4d2PointProjection);
            break;

        case UI_KEY_BACKSPACE:
            setTimerCallback(1, g4dRotateViewportAngle, NULL);
            break;

        case UI_KEY_ENTER:
            engDropSolid(pEngGame);
            break;
        }
    }
}
