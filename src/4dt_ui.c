/**
 * \file  4dt_ui.c
 * \brief Modul of user interface event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_ai.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_scn.h"
#include "4dt_menu.h"

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
void uiKeyPress(int key)
{
  // if game menu is activated
  if(menuActived())
  {
    // Forward user action to menu module
    // Check which key pressed.
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
    // Check which key pressed.
    switch (key)
    {
      // If escape
      case UI_KEY_ESC:
        // go to the menu
        menuNavigate(eMenuForward);
        break;

      // If up arrow then
      case UI_KEY_UP:
        // rotate viewport around x axis
        // to positive direction.
        g4dRotateViewport(eM4dAxisY, eM4dAxisZ, -uiViewportRotAngle);
		break;

      // If up arrow then
      case UI_KEY_LEFT:
        // rotate viewport around z axis
        // to negative direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisY, -uiViewportRotAngle);
      break;

      // If up arrow then
      case UI_KEY_DOWN:
        // rotate viewport around x axis
        // to positive direction.
        g4dRotateViewport(eM4dAxisY, eM4dAxisZ, uiViewportRotAngle);
      break;

      // If up arrow then
      case UI_KEY_RIGHT:
        // rotate viewport around z axis
        // to negative direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisY, uiViewportRotAngle);
      break;

	  //****   MOD - Just fiddling around with rotations  ****//
	  case 'q':
        // rotate viewport around y axis
        // to positive direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisZ, -uiViewportRotAngle);
		break;

	  case 'a':
        // rotate viewport around y axis
        // to negative direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisZ, uiViewportRotAngle);
		break;

	  case 'w':
        // rotate viewport around w axis(?)
        // to positive direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisW, -uiViewportRotAngle);
		break;

	  case 's':
        // rotate viewport around w axis(?)
        // to negative direction.
        g4dRotateViewport(eM4dAxisX, eM4dAxisW, uiViewportRotAngle);
		break;

	  case 'e':
        // rotate viewport around w axis(?)
        // to positive direction.
        g4dRotateViewport(eM4dAxisY, eM4dAxisW, -uiViewportRotAngle);
		break;

	  case 'd':
        // rotate viewport around w axis(?)
        // to negative direction.
        g4dRotateViewport(eM4dAxisY, eM4dAxisW, uiViewportRotAngle);
		break;

	  case 'r':
        // rotate viewport around w axis(?)
        // to positive direction.
        g4dRotateViewport(eM4dAxisZ, eM4dAxisW, -uiViewportRotAngle);
		break;

	  case 'f':
        // rotate viewport around w axis(?)
        // to negative direction.
        g4dRotateViewport(eM4dAxisZ, eM4dAxisW, uiViewportRotAngle);
		break;

		//**** END MOD ****//

      // If 'z' then
      case 'z':
        // turn the solid
        // around axis 2.
        engTurn(0, 1);
      break;

      // If 'x' then
      case 'x':
        // turn the solid
        // around axis 0.
        engTurn(1, 2);
      break;

      // If 'c' then
      case 'c':
        // turn the solid
        // around axis 1.
        engTurn(2, 0);
      break;

      // If 'v' then
      case 'v':
        // turn the solid
        // around axis 4.
        engTurn(0, 3);
      break;

      // If space then
      case ' ':
        // step down the solid.
        engLowerSolid();
      break;

      case '1':
          scnAxle = 0;
      break;

      case '2':
          scnAxle = 1;
      break;

      case '3':
          scnAxle = 2;
      break;

      case UI_KEY_PGUP:
      case '+':
      {
        // turn around selected axle clockwise
        engTurn((scnAxle == 0) ? 1 : 0, (scnAxle == 2) ? 1 : 2);
      }
      break;

      case UI_KEY_PGDOWN:
      case '-':
      {
        // turn around selected axle counterclockwise
        engTurn((scnAxle == 2) ? 1 : 2, (scnAxle == 0) ? 1 : 0);
      }
      break;

      case UI_KEY_HOME:
        // turn around selected axle clockwise
        engTurn(scnAxle, 3);
      break;

      case UI_KEY_END:
        // turn around selected axle counterclockwise
        engTurn(3, scnAxle);
      break;

      case 'A':
        // Switch the autoplayer on.
        aiAutoGamerON = !aiAutoGamerON;
      break;
    }
  }
}
