/**
 * \file  4dt_ui.c
 * \brief Modul of user interface event handlers
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include "stdlib.h"

#include "4dt_eng.h"
#include "4dt_ai.h"
#include "4dt_g3d.h"
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
  if(menuActive())
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
        g3dAngleX += g3dDAngle;
      break;

      // If up arrow then
      case UI_KEY_LEFT:
        // rotate viewport around z axis
        // to negative direction.
        g3dAngleZ += g3dDAngle;
      break;

      // If up arrow then
      case UI_KEY_DOWN:
        // rotate viewport around x axis
        // to positive direction.
        g3dAngleX -= g3dDAngle;
      break;

      // If up arrow then
      case UI_KEY_RIGHT:
        // rotate viewport around z axis
        // to negative direction.
        g3dAngleZ -= g3dDAngle;
      break;

      // If 'q' then
      case 'q':
        // exit from the software
        exit(EXIT_SUCCESS);
      break;

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
        // Set game difficulty to easy.
        engGE.game_opts.diff = 0;
      break;

      case '2':
        // Set game difficulty to medium.
        engGE.game_opts.diff = 1;
      break;

      case '3':
        // Set game difficulty to hard.
        engGE.game_opts.diff = 2;
      break;

      case 'a':
        // Switch the autoplayer on.
        aiAutoGamerON = !aiAutoGamerON;
      break;
    }
  }
}
