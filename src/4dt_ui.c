/**
 * \file  4dt_ui.c
 * \brief Modul of user interface event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>

#include "4dt_m.h"
#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_ai.h"
#include "4dt_scn.h"
#include "4dt_g4d.h"
#include "4dt_menu.h"
#include "4dt_timer.h"

#include "4dt_ui.h"

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
      case 'z':
        /*  turn the solid */
        /*  around axis 2. */
        engTurn(0, 1, 1, 1, pEngGame);
      break;
      case 'Z':
        engTurn(0, 1, -1, 1, pEngGame);
      break;

      /*  If 'x' then */
      case 'x':
        /*  turn the solid */
        /*  around axis 0. */
        engTurn(1, 2, 1, 1, pEngGame);
      break;
      case 'X':
        engTurn(1, 2, 1, 1, pEngGame);
      break;

      /*  If 'c' then */
      case 'c':
        /*  turn the solid */
        /*  around axis 1. */
        engTurn(2, 0, 1, 1, pEngGame);
      break;
      case 'C':
        engTurn(2, 0, -1, 1, pEngGame);
      break;

      /*  If 'v' then */
      case 'v':
        /*  turn the solid */
        /*  around axis 4. */
        engTurn(0, 3, 1, 1, pEngGame);
      break;
      case 'V':
        engTurn(0, 3, -1, 1, pEngGame);
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
        engTurn((pScnSet->axle == 0) ? 1 : 0,
                (pScnSet->axle == 2) ? 1 : 2,
                1, 1, pEngGame);
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

      case UI_KEY_HOME:
        /*  turn around selected axle clockwise */
        engTurn(pScnSet->axle, 3,
                1, 1, pEngGame);
      break;

      case UI_KEY_END:
        /*  turn around selected axle counterclockwise */
        engTurn(3, pScnSet->axle,
                1, 1, pEngGame);
      break;

      case 'a':
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
