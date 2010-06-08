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

#include <string.h>
#include <GL/glut.h>

#include "4dt_m4d.h"
#include "4dt_ai.h"
#include "4dt_eng.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_scn.h"
#include "4dt_ui.h"
#include "4dt_menu.h"
#include "4dt_hst.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

const int animationTimeStep = 25;
const int rotationTimeStep = 25;

static int debugmode = 0;
/*
--------------------------------------------------------------------------------
   PROTOTYPES
--------------------------------------------------------------------------------
*/

static void specialKeyPress(int key, int x, int y);
static void keyPress(unsigned char key, int x, int y);
static void engineTimerCallback(int value);
static void autoplayTimerCallback(int value);
static void animationTimerCallback(int value);
static void rotationTimerCallback(int value);
static void processARGV(int argc, char *argv[]);

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
    Event handlers
*/

/** Timer for view rotation when no player */
static void rotationTimerCallback(int value)
{
  if (m3dRotateViewport())
  {
    glutTimerFunc(rotationTimeStep, rotationTimerCallback, value);

    // Force drawing
    glutPostRedisplay();
  }
}

/** Timer for animations */
static void animationTimerCallback(int value)
{
  int moreCallNeeded;

  // apply animation step
  moreCallNeeded = engAnimation();

  // set call back again
  if (moreCallNeeded)
  {
    glutTimerFunc(animationTimeStep, animationTimerCallback, value);
  }

  // Force drawing
  glutPostRedisplay();
}

/** Timer function for Game engine. */
static void engineTimerCallback(int value)
{
  static int gameOverDetected = 0;

  if (engGE.gameOver == 0)
  {
    gameOverDetected = 0;

    if (!menuActived() || aiAutoGamerON)
    {
      if (debugmode) { engPrintSpace(); }

      // lower the solid and
      engLowerSolid();
    }
  }
  else
  {
    if (gameOverDetected != 1)
    {
      glutTimerFunc(rotationTimeStep, rotationTimerCallback, value);
      menuGotoItem(eMenuGameOver);
      gameOverDetected = 1;
    }
  }

  // Force drawing
  glutPostRedisplay();

  // set call back again
  glutTimerFunc(engGetTimestep(), engineTimerCallback, value);

  // set animation callback
  glutTimerFunc(animationTimeStep, animationTimerCallback, 0);
}

/** Timer function for Autoplayer. */
static void autoplayTimerCallback(int value)
{
  if ((engGE.gameOver == 0) && (aiAutoGamerON))
  {
    if (debugmode) { engPrintSpace(); }
    aiDoStep();
  }

  // Force drawing
  glutPostRedisplay();

  // set call back again
  glutTimerFunc(aiTimeStep, autoplayTimerCallback, value);

  // set animation callback
  glutTimerFunc(animationTimeStep, animationTimerCallback, 0);
}

/** Eventhandler of special key pressing. */
static void specialKeyPress(int key, int x, int y)
{
  int uiKey;

  switch (key)
  {
  case GLUT_KEY_UP:        uiKey = UI_KEY_UP;     break;
  case GLUT_KEY_LEFT:      uiKey = UI_KEY_LEFT;   break;
  case GLUT_KEY_DOWN:      uiKey = UI_KEY_DOWN;   break;
  case GLUT_KEY_RIGHT:     uiKey = UI_KEY_RIGHT;  break;
  case GLUT_KEY_PAGE_UP:   uiKey = UI_KEY_PGUP;   break;
  case GLUT_KEY_PAGE_DOWN: uiKey = UI_KEY_PGDOWN; break;
  case GLUT_KEY_HOME:      uiKey = UI_KEY_HOME;   break;
  case GLUT_KEY_END:       uiKey = UI_KEY_END;    break;
  }

  uiKeyPress(uiKey);

  // Refresh the display.
  glutPostRedisplay();

  // set animation callback
  glutTimerFunc(animationTimeStep, animationTimerCallback, 0);
}

/** Eventhandler of key pressing. */
static void keyPress(unsigned char key, int x, int y)
{
  uiKeyPress(key);

  // Refresh the display.
  glutPostRedisplay();

  // set animation callback
  glutTimerFunc(animationTimeStep, animationTimerCallback, 0);
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

/*------------------------------------------------------------------------------
    M A I N
*/

/** Main function of the software */
int main(int argc, char *argv[])
{
  processARGV(argc, argv);

  // Initialize/load High Score table
  hstInit();

  // Set random colors for game levels
  scnInit();

  // Initialize the game engine.
  engInitGame();

  // Set the size of the window.
  glutInitWindowSize(640,480);

  // Set the position of the window's top left corner.
  glutInitWindowPosition(50,50);

  // Initialise Glut.
  glutInit(&argc, argv);

  // Set Glut display mode.
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

  // Create the window with the specified title.
  glutCreateWindow("4DTris");

  // Set the reshape function.
  glutReshapeFunc(g3dResize);

  // initialise 3D drawing modul
  g3dInit();

  // initialise 4D drawing modul
  g4dCalibrate(SPACELENGTH);

  // Set the display function.
  glutDisplayFunc(scnDisplay);

  // Set the keypress event handler function.
  glutKeyboardFunc(keyPress);

  // Set the special keypress event handler function.
  glutSpecialFunc(specialKeyPress);

  // set timer callbacks
  glutTimerFunc(engGetTimestep(), engineTimerCallback, 0);
  glutTimerFunc(aiTimeStep, autoplayTimerCallback, 0);
  glutTimerFunc(rotationTimeStep, rotationTimerCallback, 0);

  // Start glut's main loop.
  glutMainLoop();

  // Return with successed exit constant.
  return EXIT_SUCCESS;
}
