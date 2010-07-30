/**
 * \file  4dt_g4d.c
 * \brief 4 dimensional drawing module.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <math.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"

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

/** Viewport orientation matrix */
static tM4dMatrix g4dViewport;

/** Enable flag for auto viewprot rotation */
static int g4dAutoRotationEnabled = 1;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static double g4dPerspFact(double w);

static void g4dDrawPoly(tM4dVector points[4],
                        float color[4],
                        int enableWire);

static tM3dVector g4dProject(tM4dVector vector);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


/** Initialize the 4D drawing unit. */
void g4dInit(double w_maximum)
{
  g4dMaxW = w_maximum;

  g4dReset();
}

/** Reset 4D draving unit */
void g4dReset(void)
{
  g4dViewport = m4dUnitMatrix();
}

void g4dSwitchAutoRotation(int enable)
{
  g4dAutoRotationEnabled = enable;
}

/** View rotation procedure. Should be triggered. */
int g4dAutoRotateViewport(void)
{
  if (g4dAutoRotationEnabled)
  {
    g4dViewport = m4dMultiplyMM(m4dRotMatrix(eM4dAxisY, eM4dAxisZ,
                                             0.25 * M_PI / 180),
                                g4dViewport);

    g4dViewport = m4dMultiplyMM(m4dRotMatrix(eM4dAxisX, eM4dAxisY,
                                             0.20 * M_PI / 180),
                                g4dViewport);
    return(1);
  }
  else
  {
    return(0);
  }
}

/** Rotates viewport with givel angle */
void g4dRotateViewport(eM4dAxis axis1, eM4dAxis axis2, double angle)
{
  g4dViewport = m4dMultiplyMM(m4dRotMatrix(axis1, axis2, angle),
                              g4dViewport);
}

//void g4dAddToViewport(int Col, int Row, double Value) { //MOD
//  g4dViewport.c[Col][Row] += Value;
//}

static tM3dVector g4dProject(tM4dVector vector)
{
  tM3dVector result;

  eM3dAxis axis;

  vector = m4dMultiplyMV(g4dViewport, vector);

  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    result.c[axis] = vector.c[axis] * g4dPerspFact(vector.c[eM4dAxisW]);
  }

  return(result);
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
void g4dDrawLine(tM4dVector point0,
                 tM4dVector point1,
                 float color0[4],
                 float color1[4],
                 float linewidth)
{
  g3dDrawLine(g4dProject(point0),
              g4dProject(point1),
              color0, color1,
              linewidth);
}


/** \brief Draws 4D polygon */
static void g4dDrawPoly(tM4dVector points[4],
                        float color[4],
                        int mode)
{
  tM3dVector points3D[4];
  int i;

  for (i = 0; i < 4; i++)
  {
    points3D[i] = g4dProject(points[i]);
  }

  g3dDrawPoly(points3D, color, mode);
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
  int n, i, k;
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
    tM4dVector pointlist[4];

    for (k = 0; k < 4; k++)
    {
      pointlist[k] = points[faces[i][k]];
    }

    g4dDrawPoly(pointlist, color, mode);
  }
}
