/*
4D-TRIS - Tetris like game in four dimension
Copyright (C) 2008 Simon, Laszlo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*//*
 Author: Simon, Laszlo <laszlo.simon@gmail.com>

 Short Description:
 This program is a tetris game uses 4D hypercubes.
 This file is the main unit of the application.

 ToDo:
*/

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <GL/gl.h> 
#include <GL/glu.h>
#include <GL/glut.h>

// Include standard library.
#include <stdlib.h>

#include <stdio.h>

#include "4dt_ai.h"
#include "4dt_eng.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

// Struct of game engine:
extern t_game_Engine ge;

// Variables:

// flag for auto gamer
static int autoGamer_ON = 0;

// time ellapsed while the the solid steps one level down;
static double timestep = 1.5;

// time ellapsed while the AI steps the next turn;
static double timestepAI = 0.15;

// initial direction of the viewPort (deg);
static double angleX = -60.0;
static double angleZ = 10.0;

// changes of the viewPort direction by one keypress (deg);
static double dangle = 10.0;

// Color of the 4D cube.
float cube4d_color[4] = {0.4, 0.4, 0.6, 0.15};

// array of the colors of game space levels.
static GLfloat level_colors[SPACELENGTH][4];


/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/


/*------------------------------------------------------------------------------
    VISUALISATION
*/

// Draw bitmap text to the specified coordinates and font.
void renderString(double x, double y, double z, void *font, char *string)
{
  // Local variables:
  char *c; // actual character.

  // Set position in 3D.
  glRasterPos3d(x, y, z);

  // For each character of the string
  for (c=string; *c != '\0'; c++)
  {
    // draw the character.
    glutBitmapCharacter(font, *c);
  }
}

// Write out the score to the game window.
static void writeScore(void)
{
  // Local variables:
  char text[30];     // buffer for the full text;
  int i;             // loop counter;
  int score;         // score value.

  // Set color for the text.
  glColor4d(0.2, 0.2, 0.4, 0.8);

  // Create the score character array.
  sprintf(text, "Score: %i", ge.score);

  // Render the text.
  renderString(-3.5, -2.5, -6.0, GLUT_BITMAP_9_BY_15, text);
}

// Main drawing function.
static void display(void)
{
  // Local variables:
  int l, x, y, z;        // loop counter;

  // Clear the display buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  // Write out the game score.
  writeScore();

  // Place and orient the viewport.
  glTranslated(0, 0, -6);
  glRotated(angleX, 1, 0, 0);
  glRotated(angleZ, 0, 0, 1);

  // Draw the gamespace.

  // For each level
  for (l = 0; l < SPACELENGTH; l++)
  {
    // For each cell of the level
    for (x = 0; x <= 1; x++)
    for (y = 0; y <= 1; y++)
    for (z = 0; z <= 1; z++)
    {
      // if the cell is not empty then
      if (!!getSpaceCell(l, x, y, z))
      {
        // draw the cube.
        g4dDraw4DCube(x - 1, y - 1, z - 1, 1 + l, level_colors[l], 4, 0);
      }
    }
  }

  // Draw the bottom level.

  // For each cell of the level do:
  for (x = 0; x <= 1; x++)
  for (y = 0; y <= 1; y++)
  for (z = 0; z <= 1; z++)
  {
    g4dDraw4DCube(x - 1, y - 1, z - 1, 0, cube4d_color, 3, 1);
  }

  // Draw the actual solid.

  // For each cell
  for (l = 0; l < 2; l++)
  {
    for (x = 0; x <= 1; x++)
    for (y = 0; y <= 1; y++)
    for (z = 0; z <= 1; z++)
    {
      // if the cell is not empty then
      if (!!getSolidCell(l, x, y, z))
      {
        // draw the hypercube.
        g4dDraw4DCube(x - 1, y - 1, z - 1, ge.pos + l, cube4d_color, 4, 1);
      }
    }
  }

  glPopMatrix();

  // Swap the buffers.
  glutSwapBuffers();
}


/*------------------------------------------------------------------------------
    EVENT HANDLING
*/
// Eventhandler of key pressing.
static void key(unsigned char key, int x, int y)
{
  // Check which key pressed.
  switch (key)
  {
    // If 'Esc' or 'q' then
    case 27 :
    case 'q':
      // exit from the software
      exit(0);
    break;

    // If 'z' then
    case 'z':
      // turn the solid
      // around axis 2.
      turn(0, 1);
    break;

    // If 'x' then
    case 'x':
      // turn the solid
      // around axis 0.
      turn(1, 2);
    break;

    // If 'c' then
    case 'c':
      // turn the solid
      // around axis 1.
      turn(2, 0);
    break;

    // If 'v' then
    case 'v':
      // turn the solid
      // around axis 4.
      turn(0, 3);
    break;

    // If space then
    case ' ':
      // step down the solid.
      lowerSolid();
    break;

    case '1':
      // Set game difficulty to easy.
      ge.game_opts.diff = 0;
    break;

    case '2':
      // Set game difficulty to medium.
      ge.game_opts.diff = 1;
    break;

    case '3':
      // Set game difficulty to hard.
      ge.game_opts.diff = 2;
    break;

    case 'a':
      // Switch the autoplayer on.
      autoGamer_ON = !autoGamer_ON;
    break;

  }


  // Refresh the display.
  glutPostRedisplay();
}

// Eventhandler of special key pressing.
static void special(int key, int x, int y)
{
  // Check which key pressed.
  switch (key)
  {
    // If up arrow then
    case GLUT_KEY_UP:
      // rotate viewport around x axis
      // to positive direction.
      angleX += dangle;
    break;
    // If up arrow then
    case GLUT_KEY_LEFT:
      // rotate viewport around z axis
      // to negative direction.
      angleZ += dangle;
    break;
    // If up arrow then
    case GLUT_KEY_DOWN:
      // rotate viewport around x axis
      // to positive direction.
      angleX -= dangle;
    break;
    // If up arrow then
    case GLUT_KEY_RIGHT:
      // rotate viewport around z axis
      // to negative direction.
      angleZ -= dangle;
    break;
  }

  // Refresh the display.
  glutPostRedisplay();
}

// Idle process.
static void idle(void)
{

  // Time storage for lower down the solid and AI step
  // initialised with ellapsed time since program started.
  static double timeLower = 0.0;
  static double timeAI = 0.0;

  // Time ellapsed since program started.
  const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

  // If time ellapsed since last storage larger the time of step down,
  if (t - timeLower > timestep) {
    // lower the solid and
    lowerSolid();
    // store the actual time.
    timeLower = t;
  }

  // If time ellapsed since last storage larger the time of AI step,
  // and auto player is switched on,
  if ((t - timeAI > timestepAI) && (autoGamer_ON)) {
    // make the AI step and
    dostep();
    // store the actual time.
    timeAI = t;
  }

  // Refresh the display.
  glutPostRedisplay();
}

/*------------------------------------------------------------------------------
    INITIALISATION
*/

/** \brief Set random colors for game levels */
static void initLevelColors(void)
{
  // Loop counters.
  int i, j;

  // For each level of the game space,
  for (i = 0; i < SPACELENGTH; i++)
  {
    // for each color component
    for (j = 0; j < 3; j++)
    {
      // create a random value.
      level_colors[i][j] = (double)rand() / RAND_MAX;
    }
    // Set the color's alpha component.
    level_colors[i][3] = 1.0;
  }
}

// Main function of the software
int main(int argc, char *argv[])
{
  // Game option structure for initialize game engine.
  t_game_opts in_game_opts;

  // Set game difficulty parameter.
  in_game_opts.diff = 2;

  // Initialize the game engine.
  initGame(in_game_opts);

  // Set random colors for game levels
  initLevelColors();

  g3dInit(argc, argv);

  g4dCalibrate(SPACELENGTH);

  // Set the display function.
  glutDisplayFunc(display);
  // Set the keypress event handler function.
  glutKeyboardFunc(key);
  // Set the special keypress event handler function.
  glutSpecialFunc(special);
  // Set the idle function.
  glutIdleFunc(idle);

  // Start glut's main loop.
  glutMainLoop();

  // Return with successed exit constant.
  return EXIT_SUCCESS;
}
