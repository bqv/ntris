/**
 * \file  4dt_scn.c
 * \brief Modul for handling game scene
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

// Include standard library.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_menu.h"

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

/** Flag indicates if hypercube should be drawn or only the "top side" cube */
static int scnEnableHypercubeDraw = 1;

/** Flag indicates if block should be drawn separated */
static int scnEnableSeparateBlockDraw = 0;

/** Flag indicates if grid has to be drawn */
static int scnEnableGridDraw = 0;

/** Color of the 4D cube. */
static float scn4DCubeColor[4] = {0.4, 0.4, 0.6, 0.08};
/** Color of the 4D grid. */
static float scn4DGridColor[4] = {0.4, 0.4, 0.6, 0.015};

/** array of the colors of game space levels. */
static float scnLevelColors[SPACELENGTH][4];

/** selected axle for rotation */
int scnAxle = 0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void scnDrawBG(void);
static void scnWriteScore(void);
static void scnInitLevelColors(void);
static void scnDrawRotAxis(void);
static void scnVisibleSides(int n, int (*visibleSides)[eM4dDimNum][2]);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Set function for hypercube draw enable flag */
void scnSetEnableHypercubeDraw(int enable) { scnEnableHypercubeDraw = enable; }
/** Get function for hypercube draw enable flag */
int scnGetEnableHypercubeDraw(void) { return(scnEnableHypercubeDraw); }

/** Set function for separate block draw enable flag */
void scnSetEnableSeparateBlockDraw(int enable) { scnEnableSeparateBlockDraw = enable; }
/** Get function for separate block draw enable flag */
int scnGetEnableSeparateBlockDraw(void) { return(scnEnableSeparateBlockDraw); }

/** Set function for grid draw enable flag */
void scnSetEnableGridDraw(int enable) { scnEnableGridDraw = enable; }
/** Get function for grid draw enable flag */
int scnGetEnableGridDraw(void) { return(scnEnableGridDraw); }

/** \brief Set random colors for game levels */
static void scnInitLevelColors(void)
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
      scnLevelColors[i][j] = (double)rand() / RAND_MAX;
    }
    // Set the color's alpha component.
    scnLevelColors[i][3] = 1.0;
  }
}

/** Write out the score to the game window. */
static void scnWriteScore(void)
{
  // Local variables:
  char text[30];     // buffer for the full text;
  float color[4] = {0.3, 0.3, 0.5, 1.0};

  // Create the score character array.
  sprintf(text, "Score: %i", engGE.score);

  // Render the text.
  g3dRenderString(0.1, 0.1, color, text);
}

/** Draw background for scene */
static void scnDrawBG(void)
{
  float color1[4] = {1.0, 1.0, 1.0, 1.0};
  float color2[4] = {0.8, 0.8, 0.8, 1.0};

  // Draw background
  g3dDrawRectangle(0.0, 0.0, 1.0, 1.0, color1, color2);
}

/** draws the rotation axis selected */
static void scnDrawRotAxis(void)
{
  int i;
  const double planeSize = 3.5;

  float color0[4] = {0.0, 0.0, 0.0, 0.8};
  float color1[4] = {0.0, 0.0, 0.0, 0.0};

  if ((scnAxle <= 2) && (scnAxle >= 0))
  {
    for (i = -1; i <= 1; i += 2)
    {
      tM4dVector point0 = m4dNullVector();
      tM4dVector point1 = m4dUnitVector(scnAxle);

      point1 = m4dMultiplySV(i * planeSize, point1);

      point0.c[eM4dAxisW] = point1.c[eM4dAxisW] = engGE.object.pos.c[eM4dAxisW];

      g4dDrawLine(point0, point1, color0, color1, 2.0);
    }
  }

  g4dDrawLine(m4dNullVector(),
              m4dMultiplySV(10, m4dUnitVector(eM4dAxisW)),
              color0, color1, 1.0);
}

/** Initialize the scene */
void scnInit(void)
{
  // Initialize random number generator with the
  // Answer to the Ultimate Question of Life, the Universe, and Everything
  // to get always the same colors for levels.
  srand(42);

  scnInitLevelColors();
}

/**  Check, which side of the block should not be drawn because have neighbor.
 * n - index of the block in object
 * visibleSides - return array */
static void scnVisibleSides(int n, int (*visibleSides)[eM4dDimNum][2])
{
  int i;

  for (i = eM4dAxisX; i < eM4dDimNum; i++)
  {
    (*visibleSides)[i][0] = 1;
    (*visibleSides)[i][1] = 1;
  }

  for (i = 0; i < engGE.object.block.num; i++)
  {
    /* indicates, which orientation 'i' block is neighbor with 'n'
       -1 = not found, -2 = no neighbor */
    eM4dAxis orientation = -1;
    eM4dAxis axis;

    tM4dVector sub =
      m4dSubVectors(engGE.object.block.c[i],engGE.object.block.c[n]);

    for (axis = eM4dAxisX; axis < eM4dDimNum; axis++)
    {
      if (abs(sub.c[axis]) >= 0.5)
      {
        orientation = (orientation == -1)
                      ? axis
                      : -2;
      }
    }

    if (orientation >= 0)
    {
      (*visibleSides)[orientation]
                     [(sub.c[orientation] > 0) ? 1 : 0] = 0;
    }
  }
}

/** Main drawing function. */
void scnDisplay(void)
{
  // Local variables:
  int n, l, x, y, z;        // loop counter;

  // mask indicates which block space
  // hidden by upper blocks
  int mask[2][2][2] = {{{0,0},{0,0}},{{0,0},{0,0}}};

  g3dBeginDraw();

  scnDrawBG();

  // Write out the game score.
  scnWriteScore();

  // Draw the gamespace.

  // For each level from top
  for (l = SPACELENGTH - 1; l >= 0; l--)
  {
    // For each cell of the level
    for (x = 0; x <= 1; x++)
    for (y = 0; y <= 1; y++)
    for (z = 0; z <= 1; z++)
    {
      // space which has no cube above (so it is visible)
      // gets rid of Z-fighting
      if (
1 ||
          mask[x][y][z] == 0)
      {
        // if the cell is not empty then
        if (engGetSpaceCell(l, x, y, z))
        {
          // draw the cube.
          g4dDraw4DCube(m4dVector(x - 0.5, y - 0.5, z - 0.5, l - 0.5),
                        m4dUnitMatrix(),
                        scnLevelColors[l],
                        scnEnableHypercubeDraw ? 4 : 3, 0, NULL);

          mask[x][y][z] = 1;
        }
        else
        {
          if (scnEnableGridDraw)
          {
            g4dDraw4DCube(m4dVector(x - 0.5, y - 0.5, z - 0.5, l - 0.5),
                          m4dUnitMatrix(),
                          scn4DGridColor, 4, 2, NULL);
          }
        }
      }
    }
  }

  // Draw the bottom level.

  // For each cell of the level do:
  for (x = 0; x <= 1; x++)
  for (y = 0; y <= 1; y++)
  for (z = 0; z <= 1; z++)
  {
    // space which has no cube above (so it is visible)
    if (mask[x][y][z] == 0)
    {
      g4dDraw4DCube(m4dVector(x - 0.5, y - 0.5, z - 0.5, -0.5),
                    m4dUnitMatrix(),
                    scn4DCubeColor, 3, 1, NULL);
    }
  }

  // Draw the actual solid.

  // For each cell
  for (n = 0; n < engGE.object.block.num; n++)
  {
    tM4dVector pos;
    int visibleSides[eM4dDimNum][2];

    scnVisibleSides(n, &visibleSides);

    pos = m4dAddVectors(engGE.object.pos,
                        m4dMultiplyMV(engGE.object.axices,
                                      engGE.object.block.c[n]));

    // draw the hypercube.
    g4dDraw4DCube(pos, engGE.object.axices, scn4DCubeColor,
                  scnEnableHypercubeDraw ? 4 : 3, 1,
                  scnEnableSeparateBlockDraw ? NULL : visibleSides);
  }

  scnDrawRotAxis();

  // draw the menu
  if (menuIsActived())
  {
    menuDraw();
  }

  g3dEndDraw();
}
