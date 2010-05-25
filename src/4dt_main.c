/**
   \mainpage 4DTRIS - Tetris like game in four dimension
   \image html 4DTRIS_icon_128.png

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

#include <GL/gl.h> 
#include <GL/glu.h>
#include <GL/glut.h>


#include "4dt_ai.h"
#include "4dt_eng.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_scn.h"
#include "4dt_ui.h"
#include "4dt_menu.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
   PROTOTYPES
--------------------------------------------------------------------------------
*/

static void idle(void);
static void specialKeyPress(int key, int x, int y);
static void keyPress(unsigned char key, int x, int y);

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
    Event handlers
*/


/** Idle process. */
static void idle(void)
{
  double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

  if (!menuActive() || aiAutoGamerON)
  {
    engTrigger(time);
  }

  aiTrigger(time);

  glutPostRedisplay();
}

/** Eventhandler of special key pressing. */
static void specialKeyPress(int key, int x, int y)
{
  int uiKey;

  switch (key)
  {
  case GLUT_KEY_UP:    uiKey = UI_KEY_UP;    break;
  case GLUT_KEY_LEFT:  uiKey = UI_KEY_LEFT;  break;
  case GLUT_KEY_DOWN:  uiKey = UI_KEY_DOWN;  break;
  case GLUT_KEY_RIGHT: uiKey = UI_KEY_RIGHT; break;
  }

  uiKeyPress(uiKey);

  // Refresh the display.
  glutPostRedisplay();
}

/** Eventhandler of key pressing. */
static void keyPress(unsigned char key, int x, int y)
{
  uiKeyPress(key);

  // Refresh the display.
  glutPostRedisplay();
}


/*------------------------------------------------------------------------------
    M A I N
*/

/** Main function of the software */
int main(int argc, char *argv[])
{
  // Initialize the game engine.
  engInitGame();

  // Set random colors for game levels
  scnInit();

  g3dInit(argc, argv);

  g4dCalibrate(SPACELENGTH);

  // Set the display function.
  glutDisplayFunc(scnDisplay);
  // Set the keypress event handler function.
  glutKeyboardFunc(keyPress);
  // Set the special keypress event handler function.
  glutSpecialFunc(specialKeyPress);
  // Set the idle function.
  glutIdleFunc(idle);

  // Start glut's main loop.
  glutMainLoop();

  // Return with successed exit constant.
  return EXIT_SUCCESS;
}
