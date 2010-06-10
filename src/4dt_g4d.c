/**
 * \file  4dt_g4d.c
 * \brief 4 dimensional drawing module.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include "4dt_g3d.h"
#include "4dt_m4d.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Size of cube represent the lowest level. */
static const double g4dBaseSize = 0.75;
/** Size of cube represent the highest level. */
static const double g4dMaxSize = 2.0;

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Maximal coordinate on 4th axis */
static double g4dMaxW;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static double g4dPerspFact(double w);

static void g4dDrawPoly(float points[4][4],
                        float color[4],
                        int enableWire);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


/** Calibrate/initialize the 4D drawing unit. */
void g4dCalibrate(double w_maximum)
{
  g4dMaxW = w_maximum;
}

/** \brief Calculates perspective projection factor of the 'level'
           (game space 4th axis). */
static double g4dPerspFact(double w)
{
  // Local variables:
  double baseLevel, // lowest level if the viewpoint is at zero level;
         sizeFact,  // factor of highest and lowest level projected size;
         result;    // return value.

  // Calculate size factor.
  sizeFact = g4dBaseSize / g4dMaxSize;

  // Calculate viewpoint distance from lowest level.
  baseLevel = g4dMaxW * sizeFact / (1 - sizeFact);

  // Calculate perspective projection.
  result = g4dBaseSize * (baseLevel + g4dMaxW) /
           (baseLevel + g4dMaxW - w);

  // Return with the result value.
  return result;
}

/** Draws 4D line */
void g4dDrawLine(float point0[4],
                 float point1[4],
                 float color0[4],
                 float color1[4],
                 float linewidth){
  int n;

  for (n = 0; n < 3; n++)
  {
    point0[n] = point0[n] * g4dPerspFact(point0[3]);
    point1[n] = point1[n] * g4dPerspFact(point1[3]);
  }

  g3dDrawLine(point0, point1,
              color0, color1,
              linewidth);
}


/** \brief Draws 4D polygon */
static void g4dDrawPoly(float points[4][4],
                 float color[4],
                 int mode)
{
  int i, n;

  for (i = 0; i < 4; i++)
  for (n = 0; n < 3; n++)
  {
    points[i][n] = points[i][n] * g4dPerspFact(points[i][3]);
  }

  g3dDrawPoly(points, color, mode);
}


/** \brief Draw a 4D cube to the specified place of the game space. */
void g4dDraw4DCube(tM4dVector center,
                   tM4dMatrix orientation,
                   float color[4],
                   int dimension,
                   int mode)
{
  // Array contains the points of a 4D hypercube.
  tM4dVector points[16] =
  /*  x,   y,   z,   l */
  { {{-0.5,-0.5,-0.5,-0.5}},
    {{ 0.5,-0.5,-0.5,-0.5}},
    {{-0.5, 0.5,-0.5,-0.5}},
    {{ 0.5, 0.5,-0.5,-0.5}},
    {{-0.5,-0.5, 0.5,-0.5}},
    {{ 0.5,-0.5, 0.5,-0.5}},
    {{-0.5, 0.5, 0.5,-0.5}},
    {{ 0.5, 0.5, 0.5,-0.5}},
    {{-0.5,-0.5,-0.5, 0.5}},
    {{ 0.5,-0.5,-0.5, 0.5}},
    {{-0.5, 0.5,-0.5, 0.5}},
    {{ 0.5, 0.5,-0.5, 0.5}},
    {{-0.5,-0.5, 0.5, 0.5}},
    {{ 0.5,-0.5, 0.5, 0.5}},
    {{-0.5, 0.5, 0.5, 0.5}},
    {{ 0.5, 0.5, 0.5, 0.5}} };

  // Array contains the faces (specified with num. of the points)
  // of a 4D hypercube.
  const int faces[24][4] =
  {
// inner cube
{   8,  12,  14,  10}, {   9,  11,  15,  13}, {   8,   9,  13,  12},
{  10,  14,  15,  11}, {   8,  10,  11,   9}, {  12,  13 ,  15,  14},
// outer cube
{   0,   4,   6,   2}, {   1,   3,   7,   5}, {   0,   1,   5,   4},
{   2,   6,   7,   3}, {   0,   2,   3,   1}, {   4,   5,   7,   6},
// intermediate faces
{   0,   8, 0xC,   4}, {   0,   8, 0xA,   2}, { 0xA,   2,   6, 0xE}, { 0xE,   6,   4, 0xC},
{   1,   9, 0xB,   3}, { 0xB,   3,   7, 0xF}, {   7, 0xF, 0xD,   5}, { 0xD,   5,   1,   9},
{ 0xC, 0xD,   5,   4}, {   8,   9,   1,   0}, { 0xA, 0xB,   3,   2}, { 0xE, 0xF,   7,   6}
  };

  // Loop counter.
  int n, i, j, k;
  int facenum;

  // Move each point of the hypercube to its final position
  for (n = 0; n < 16; n++)
  {
    points[n] = m4dMultiplyMV(orientation, points[n]);
    points[n] = m4dAddVectors(points[n], center);
  }

  // Determine number of facets which has to be drawn
  facenum = (dimension == 3) ? 6 : 24;

  // For each facet and
  for (i = 0; i < facenum; i++)
  {
    float pointlist[4][4];

    for (k = 0; k < 4; k++)
    for (j = 0; j < 4; j++)
    {
      pointlist[k][j] = points[faces[i][k]].c[j];
    }

    g4dDrawPoly(pointlist, color, mode);
  }
}

