/**
 * \file  4dt_scn.c
 * \brief Modul for handling game scene
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_scn.h"
#include "4dt_g3d.h"
#include "4dt_gtxt.h"
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

/** Color of the 4D cube. */
const static float scn4DCubeColor[4] = {0.4, 0.4, 0.6, 0.35};
const static float scn4DWireColor[4] = {0.8, 0.8, 0.9, 1.00};
/** Color of the 4D grid. */
const static float scn4DGridColor[4] = {1.0, 1.0, 1.0, 0.03};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** array of the colors of game space levels. */
static float scnLevelColors[SPACELENGTH][4];

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void scnDrawBG(void);
static void scnWriteScore(int score);
static void scnInitLevelColors(void);
static void scnDrawRotAxis(int axle, tEngGame *pEngGame);
static void scnVisibleSides(int n, int (*visibleSides)[eM4dDimNum][2],
                            tEngBlocks *pEngBlock);
static void scnDrawGamespace(tEngGame *pEngGame,
                             tScnSet *pScnSet,
                             int mask[SPACESIZE][SPACESIZE][SPACESIZE]);
static void scnDrawBottomLevel(int mask[SPACESIZE][SPACESIZE][SPACESIZE],
                               int wire,
                               tEngGame *pEngGame);
static void scnDrawObject(tEngGame *pEngGame,
                          tScnSet *pScnSet,
                          int wire);
static tM4dVector scnPosToCoord(int x, int y, int z, int w,
                                tEngGame *pEngGame);
static tM4dVector scnCenter(tEngGame *pEngGame);
static void scnDrawGrid(int enableGridDraw, tEngGame *pEngGame);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Returns with the coordinates of the center of the gamespace */
static tM4dVector scnCenter(tEngGame *pEngGame)
{
  tM4dVector center = {{pEngGame->size[0]/2.0,
                        pEngGame->size[1]/2.0,
                        pEngGame->size[2]/2.0,
                        0.0}};

  return(center);
}

/** Set function for stereoscope view enable flag */
void scnSetViewMode(tScnViewMode mode, tScnSet *pScnSet)
{ pScnSet->viewMode = mode; }

/** Get function for stereoscope view draw enable flag */
tScnViewMode scnGetViewMode(tScnSet *pScnSet)
{ return(pScnSet->viewMode); }

/** Set function for hypercube draw enable flag */
void scnSetEnableHypercubeDraw(int enable, tScnSet *pScnSet)
{ pScnSet->enableHypercubeDraw = enable; }

/** Get function for hypercube draw enable flag */
int scnGetEnableHypercubeDraw(tScnSet *pScnSet)
{ return(pScnSet->enableHypercubeDraw); }

/** Set function for separate block draw enable flag */
void scnSetEnableSeparateBlockDraw(int enable, tScnSet *pScnSet)
{ pScnSet->enableSeparateBlockDraw = enable; }

/** Get function for separate block draw enable flag */
int scnGetEnableSeparateBlockDraw(tScnSet *pScnSet)
{ return(pScnSet->enableSeparateBlockDraw); }

/** Set function for grid draw enable flag */
void scnSetEnableGridDraw(int enable, tScnSet *pScnSet)
{ pScnSet->enableGridDraw = enable; }

/** Get function for grid draw enable flag */
int scnGetEnableGridDraw(tScnSet *pScnSet)
{ return(pScnSet->enableGridDraw); }

/** Returns with the default set of scene parameters */
tScnSet scnGetDefaultSet(void)
{
  tScnSet def = { 1, 0, 0, 0, eScnViewMono };

  return(def);
}

/** Converts cube position in game space to real 4D draw space coordinates */
static tM4dVector scnPosToCoord(int x, int y, int z, int w,
                                tEngGame *pEngGame)
{
  tM4dVector coords = m4dVector(x + 0.5, y + 0.5, z + 0.5, w + 0.5);

  coords = m4dSubVectors(coords, scnCenter(pEngGame));

  return (coords);
}

/** \brief Set random colors for game levels */
static void scnInitLevelColors(void)
{
  /*  Loop counters. */
  int i, j;

  /*  For each level of the game space, */
  for (i = 0; i < SPACELENGTH; i++)
  {
    /*  for each color component */
    for (j = 0; j < 3; j++)
    {
      scnLevelColors[i][j] = 0.48 + 0.24 * cos((i-0.2)+2*(2-j)*M_PI/3.0);
    }

    /*  Set the color's alpha component. */
    scnLevelColors[i][3] = 1.0;
  }
}

/** Write out the score to the game window. */
static void scnWriteScore(int score)
{
  /*  Local variables: */
  char text[30];     /*  buffer for the full text; */
  float color[4] = {0.8, 0.8, 0.9, 1.0};

  /*  Create the score character array. */
  sprintf(text, "Score: %i", score);

  /*  Render the text. */
  g3dRenderString(0.1, 0.05, color, text);
}

/** Draw background for scene */
static void scnDrawBG(void)
{
  float color1[4] = {0.3, 0.3, 0.4, 1.0};
  float color2[4] = {0.0, 0.0, 0.0, 1.0};

  /*  Draw background */
  g3dDrawRectangle(0.0, 0.0, 1.0, 0.5, color1, color2);
  g3dDrawRectangle(0.0, 0.5, 1.0, 1.0, color2, color2);
}

/** draws the rotation axis selected */
static void scnDrawRotAxis(int axle, tEngGame *pEngGame)
{
  int i;
  const double planeSize = 3.5;

  float color0[4] = {1.0, 1.0, 1.0, 0.8};
  float color1[4] = {1.0, 1.0, 1.0, 0.0};

  if ((axle <= 2) && (axle >= 0))
  {
    for (i = -1; i <= 1; i += 2)
    {
      tM4dVector point0 = pEngGame->object.pos;
      tM4dVector point1 = m4dUnitVector(axle);
      point1 = m4dMultiplySV(i * planeSize, point1);

      point0 = m4dSubVectors(point0, scnCenter(pEngGame));
      point1 = m4dAddVectors(point0, point1);

      point0.c[eM4dAxisW] =
      point1.c[eM4dAxisW] = pEngGame->object.pos.c[eM4dAxisW];

      g4dDrawLine(point0, point1, color0, color1, 2.5);
    }
  }

  g4dDrawLine(m4dNullVector(),
              m4dMultiplySV(10, m4dUnitVector(eM4dAxisW)),
              color0, color1, 1.0);
}

/** Initialize the scene */
void scnInit(void)
{
  scnInitLevelColors();
}

/**  Check, which side of the block should not be drawn because have neighbor.
 * n - index of the block in object
 * visibleSides - return array */
static void scnVisibleSides(int n, int (*visibleSides)[eM4dDimNum][2],
                            tEngBlocks *pEngBlock)
{
  int i;

  for (i = eM4dAxisX; i < eM4dDimNum; i++)
  {
    (*visibleSides)[i][0] = 1;
    (*visibleSides)[i][1] = 1;
  }

  for (i = 0; i < pEngBlock->num; i++)
  {
    /* indicates, which orientation 'i' block is neighbor with 'n'
       -1 = not found, -2 = no neighbor */
    int orientation = -1;
    eM4dAxis axis;

    tM4dVector sub =
      m4dSubVectors(pEngBlock->c[i], pEngBlock->c[n]);

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

/**  Draw the gamespace. */
static void scnDrawGamespace(tEngGame *pEngGame,
                             tScnSet *pScnSet,
                             int mask[SPACESIZE][SPACESIZE][SPACESIZE])
{
  int l, x, y, z;        /*  loop counter; */

  /*  For each level from top */
  for (l = pEngGame->spaceLength - 1; l >= 0; l--)
  {
    /*  For each cell of the level */
    for (x = 0; x < pEngGame->size[0]; x++)
    for (y = 0; y < pEngGame->size[1]; y++)
    for (z = 0; z < pEngGame->size[2]; z++)
    {
      /*  space which has no cube above (so it is visible) */
      /*  gets rid of Z-fighting */
      if (   (mask[x][y][z] == 0)
          || (g4dGetViewType() == eG4d2PointProjection) )
      {
        /*  if the cell is not empty then */
        if (engGetSpaceCell(l, x, y, z, pEngGame))
        {
          /*  draw the cube. */
          g4dDraw4DCube(scnPosToCoord(x, y, z, l, pEngGame),
                        m4dUnitMatrix(),
                        scnLevelColors[l],
                        pScnSet->enableHypercubeDraw ? 4 : 3,
                        eG3dFillAndWire, NULL);

          g4dDraw4DCube(scnPosToCoord(x, y, z, l, pEngGame),
                        m4dUnitMatrix(),
                        scnLevelColors[l],
                        pScnSet->enableHypercubeDraw ? 4 : 3,
                        eG3dWireTube, NULL);

          mask[x][y][z] = 1;
        }
      }
    }
  }
}

/** Draws grid of the gamespace */
static void scnDrawGrid(int enableGridDraw, tEngGame *pEngGame)
{
  int l;        /*  loop counter; */

  for (l = pEngGame->spaceLength - 1; l >= 0; l--)
  {
    if (enableGridDraw)
    {
      g4dDraw4DCube(m4dVector(0.0, 0.0, 0.0, l),
                    m4dMultiplyVM(m4dMultiplySV(2.0, scnCenter(pEngGame)),
                                  m4dUnitMatrix()),
                    scn4DGridColor, 4,
                    eG3dWire, NULL);
    }
  }
}

/** Draw the bottom level. */
static void scnDrawBottomLevel(int mask[SPACESIZE][SPACESIZE][SPACESIZE],
                               int wire,
                               tEngGame *pEngGame)
{
  int x, y, z;        /*  loop counter; */

  /*  For each cell of the level do: */
  for (x = 0; x < pEngGame->size[0]; x++)
  for (y = 0; y < pEngGame->size[1]; y++)
  for (z = 0; z < pEngGame->size[2]; z++)
  {
    /*  space which has no cube above (so it is visible) */
    if (mask[x][y][z] == 0)
    {
      g4dDraw4DCube(scnPosToCoord(x, y, z, 0, pEngGame),
                    m4dUnitMatrix(),
                    scn4DCubeColor, 3,
                    wire ? eG3dWireTube : eG3dFillAndWire, NULL);
    }
  }
}


/** Draw the actual solid. */
static void scnDrawObject(tEngGame *pEngGame,
                          tScnSet *pScnSet,
                          int wire)
{
  int n;        /*  loop counter; */
  /*  For each cell */
  for (n = 0; n < pEngGame->object.block.num; n++)
  {
    tM4dVector pos;
    int visibleSides[eM4dDimNum][2];

    scnVisibleSides(n, &visibleSides, &pEngGame->object.block);

    pos = m4dAddVectors(m4dSubVectors(pEngGame->object.pos,
                                      scnCenter(pEngGame)),
                        m4dMultiplyMV(pEngGame->object.axices,
                                      pEngGame->object.block.c[n]));

    /*  draw the hypercube. */
    g4dDraw4DCube(pos,
                  pEngGame->object.axices,
                  wire ? scn4DWireColor : scn4DCubeColor,
                  pScnSet->enableHypercubeDraw ? 4 : 3,
                  wire ? eG3dWireTube : eG3dFillAndWire,
                  pScnSet->enableSeparateBlockDraw ? NULL : visibleSides);
  }
}

/** Main drawing function. */
void scnDisplay(tEngGame *pEngGame, tScnSet *pScnSet)
{
  /*  Local variables: */
  int n, l, x, y, z;        /*  loop counter; */

  /*  mask indicates which block space */
  /*  hidden by upper blocks */
  int mask[SPACESIZE][SPACESIZE][SPACESIZE];

  double camx, camz;
  int pic, maxpic;

  maxpic = (pScnSet->viewMode > eScnViewMono) ? 2 : 1;

  for (pic = 0; pic < maxpic; pic++)
  {
    for (x = 0; x < pEngGame->size[0]; x++)
    for (y = 0; y < pEngGame->size[1]; y++)
    for (z = 0; z < pEngGame->size[2]; z++)
    {
      mask[x][y][z] = 0;
    }

    if (pScnSet->viewMode == eScnViewStereogram)
    {
      camx = (pic == 0) ? -2 : 2;
      camz = -12;
    }
    else
    {
      camx = 0;
      camz = -6;
    }

    g3dBeginDraw(camx, 0, camz, pic, pScnSet->viewMode);

    if ((pic == 0)
        || (pScnSet->viewMode == eScnViewAnaglyph))
    {
      scnDrawBG();
    }

    scnDrawGamespace(pEngGame, pScnSet, mask);

    scnDrawGrid(pScnSet->enableGridDraw, pEngGame);

    scnDrawBottomLevel(mask, 1, pEngGame);

    scnDrawObject(pEngGame, pScnSet, 1);

    scnDrawBottomLevel(mask, 0, pEngGame);

    scnDrawObject(pEngGame, pScnSet, 0);

    scnDrawRotAxis(pScnSet->axle, pEngGame);

    if ((pic == maxpic-1)
        || (pScnSet->viewMode == eScnViewAnaglyph))
    {
      /*  Write out the game score. */
      scnWriteScore(pEngGame->score);

      /*  draw the menu */
      if (menuIsActived())
      {
        menuDraw();
      }
    }

    if (pic == maxpic-1)
    {
      for (n = 0; n < maxpic; n++)
      {
        g3dEndDrawPic();
        g3dEndDraw();
      }
    }
  }
}
