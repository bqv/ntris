/**
 * \file  4dt_g4d.c
 * \brief 4 dimensional drawing module.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <limits.h>

#include "4dt_timer.h"
#include "4dt_m.h"
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

/** 4D W axis (projected to 1 point) coordinates in 3D
 *  when 1 point projection used */
static const tM3dVector g4dW     = {{ 0, 0, 0}};
/** 4D Origo coordinates in 3D when 2 point projection used */
static const tM3dVector g4dW0    = {{ 4, 0, 0}};
/** 4D end of W axis coordinates in 3D when 2 point projection used */
static const tM3dVector g4dWInf  = {{-20, 15, 0}};
/** Step of interpolation factor between viewmodes */
static const double g4dViewInterpolStep = 0.25;
/** Time between steps of viewmode change animation [msec] */
static const int g4dViewmodeTimeStep = 25;
/** Time steps of auto rotation [msec] */
static const int g4dRotationTimeStep = 25;

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Maximal coordinate on 4th axis */
static double g4dMaxW;

/** Viewport orientation matrix */
static tM4dMatrix g4dViewport;

/** Enable flag for auto viewprot rotation */
static int g4dAutoRotationEnabled = 0;

/** Actual View type (projection mode from 4D to 3D) */
static tG4dViewType g4dViewType = eG4d1PointProjection;
/** Interpolation factor between view modes. (used for animated mode changes) */
static double g4dViewInterpol = 0.0;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static double g4dPerspFact(double w);

static void g4dDrawPoly(tM4dVector points[4],
                        float color[4],
                        tG4dWireType wireMode,
                        int fill,
                        int sideVisible[4]);

static tM3dVector g4d2PointProject(tM4dVector vector);

static int g4dAutoRotateViewport(int interval, void *param);
static int g4dStepViewModeChange(int interval, void *param);

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
  if (g4dAutoRotationEnabled != enable)
  {
    g4dAutoRotationEnabled = enable;

    if (g4dAutoRotationEnabled == 1)
    {
      setTimerCallback(g4dRotationTimeStep, g4dAutoRotateViewport, NULL);
    }
  }
}

/** Rotates the view port with angle deg in more steps */
int g4dRotateViewportAngle(int interval, void *param)
{
  int anglestep = 30;
  static int angle = INT_MIN;

  if (angle == INT_MIN)
  {
    angle = 180;
  }

  if (angle > 0)
  {
    angle -= anglestep;
    if (angle < 0)
    {
      anglestep -= - angle;
    }

    g4dViewport = m4dMultiplyMM(m4dRotMatrix(eM4dAxisX, eM4dAxisY,
                                             anglestep * M_PI / 180),
                                g4dViewport);

    interval = g4dRotationTimeStep;
  }
  else
  {
    angle = INT_MIN;
    interval = 0;
  }

  return(interval);
}

/** View rotation procedure. Should be triggered. */
static int g4dAutoRotateViewport(int interval, void *param)
{
  if (g4dAutoRotationEnabled)
  {
    g4dViewport = m4dMultiplyMM(m4dRotMatrix(eM4dAxisY, eM4dAxisZ,
                                             0.25 * M_PI / 180),
                                g4dViewport);

    g4dViewport = m4dMultiplyMM(m4dRotMatrix(eM4dAxisX, eM4dAxisY,
                                             0.20 * M_PI / 180),
                                g4dViewport);

    return(interval);
  }
  else
  {
    return(0);
  }
}

/** View rotation procedure. Should be triggered. */
static int g4dStepViewModeChange(int interval, void *param)
{
  int newinterval = 0;

  switch(g4dViewType)
  {
    case eG4d1PointProjection:
    {
      g4dViewInterpol -= g4dViewInterpolStep;
      if (g4dViewInterpol < 0.0)
      {
        g4dViewInterpol = 0.0;
      }
      else
      {
        newinterval = interval;
      }
      break;
    }
    case eG4d2PointProjection:
    {
      g4dViewInterpol += g4dViewInterpolStep;
      if (g4dViewInterpol > 1.0)
      {
        g4dViewInterpol = 1.0;
      }
      else
      {
        newinterval = interval;
      }
      break;
    }
  }

  return(newinterval);
}

/** Set function for view type */
void g4dSetViewType(tG4dViewType viewType)
{
  g4dViewType = viewType;

  setTimerCallback(g4dViewmodeTimeStep, &g4dStepViewModeChange, 0);
}

/** Set function for view type */
tG4dViewType g4dGetViewType(void)
{
  return(g4dViewType);
}

/** Rotates viewport with givel angle */
void g4dRotateViewport(eM4dAxis axis1, eM4dAxis axis2, double angle)
{
  g4dViewport = m4dMultiplyMM(m4dRotMatrix(axis1, axis2, angle),
                              g4dViewport);
}

/**
  Picture of point on W axis             Known:
  --------------------------             O' - picture of the origo [0,0,0,0]
               \                         Q' - picture of the end of W axis
                \W axis                       [0,0,0,oo]
                 \                       W  - point on W axis [0,0,0,Ww]
                  \ W
                   o----+                Question:
          \        .\   |                W' - picture of the W point
           \        .\  | Picture of
            \       . \ | origo (O')
             \       .t\|/               (1) O'Q'CQ" - parallelogram
     <---//---o<-----o--O--- Projection
             / \    /.  |\     plane     (2) WW'O' ~ WCQ"
   picture of   \  / .  | \
   end of W axis \/   . |  \                 WO'    WQ"    WQ"
   (Q')          /\   . |   =            (3) ---- = ---- = ----
                /  \  . |    \               O'W'    Q"C   O'Q'
      picture of    =  .|     \                           ___         ___
      W point        \ .|      \             ___          O'Q'   Ww * O'Q'
      (W')            \.|       \        (4) O'W' = WO' * ---- = ---------
                       \|        \                        WQ"    Ww + O'Q"
                        o---//----+                         ___
                       /           Q"        _    _    Ww * O'Q'
                  view                   (5) W' = O' + ---------
                 point (C)                             Ww + Q'C
 */
static tM3dVector g4d2PointProject(tM4dVector vector)
{
  tM3dVector O;
  tM3dVector Q;
  tM3dVector C = {{ 0, 0, 20}};
  tM3dVector W;
  tM3dVector OQ;
  tM3dVector result;
  eM3dAxis axis;
  double Ww, dQC, temp;

  Q = m3dInterpolate(g4dW, g4dWInf, g4dViewInterpol);
  O = m3dInterpolate(g4dW, g4dW0,   g4dViewInterpol);

  OQ = m3dSub(Q, O);

  dQC = m3dAbs(m3dSub(Q, C));

  Ww = 12 - vector.c[eM4dAxisW];

  temp = Ww / (Ww + dQC);

  W = m3dAdd(O, m3dMultiplySV(temp, OQ));

  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    result.c[axis] = W.c[axis]
                     + vector.c[axis] * g4dPerspFact(vector.c[eM4dAxisW]);
  }

  return (result);
}

/** Project 4D point to the 3D space. */
tM3dVector g4dProject(tM4dVector vector)
{
  tM3dVector result;

  /*  Rotate coordinate system to the viewport coord system */
  vector = m4dMultiplyMV(g4dViewport, vector);

  result = g4d2PointProject(vector);

  return(result);
}

/** \brief Calculates perspective projection factor of the 'level'
           (game space 4th axis). */
static double g4dPerspFact(double w)
{
  /*  Local variables: */
  double baseLevel, /*  lowest level if the viewpoint is at zero level; */
         sizeFact,  /*  factor of highest and lowest level projected size; */
         result;    /*  return value. */

  /*  Calculate size factor. */
  sizeFact = g4dBaseSize / g4dMaxSize;

  /*  Calculate viewpoint distance from lowest level. */
  baseLevel = g4dMaxW * sizeFact / (1 - sizeFact);

  /*  Calculate perspective projection. */
  result = g4dBaseSize * (baseLevel + g4dMaxW) /
           (baseLevel + g4dMaxW - w);

  /*  Return with the result value. */
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

/** \brief Draws 4D Sphere */
void g4dDrawSphere(tM4dVector center, float color[4], double radius)
{
  tM3dVector center3D;

  center3D = g4dProject(center);

  g3dDrawSphere(center3D, color, radius);
}

/** \brief Draws 4D polygon */
static void g4dDrawPoly(tM4dVector points[4],
                        float color[4],
                        tG4dWireType wireMode,
                        int fill,
                        int sideVisible[4])
{
  tM3dVector points3D[4];
  int i;

  for (i = 0; i < 4; i++)
  {
    points3D[i] = g4dProject(points[i]);
  }

  if (fill)
  {
    g3dDrawPolyFill(points3D, color);
  }

  switch(wireMode)
  {
  case eG4dWireLine:    g3dDrawPolyWire(points3D, color, sideVisible); break;
  case eG4dWireTube:    g3dDrawPolyTube(points3D, color, sideVisible); break;
  case eG4dWireNone:
  default:              break;
  }
}


/** \brief Draw a 4D cube to the specified place of the game space. */
void g4dDraw4DCube(tM4dVector center,
                   tM4dMatrix orientation,
                   float color[4],
                   int dimension,
                   tG4dWireType wireMode,
                   int fill,
                   int sideVisible[eM4dDimNum][2])
{
  /*  Array contains the points of a 4D hypercube. */
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

  /*  Array contains the faces (specified with num. of the points) */
  /*  of a 4D hypercube. */
  const int faces[24][4] =
  {
/*  inner cube */
{   8,  12,  14,  10}, {   9,  11,  15,  13}, {   8,   9,  13,  12},
{  10,  14,  15,  11}, {   8,  10,  11,   9}, {  12,  13 ,  15,  14},
/*  outer cube */
{   0,   4,   6,   2}, {   1,   3,   7,   5}, {   0,   1,   5,   4},
{   2,   6,   7,   3}, {   0,   2,   3,   1}, {   4,   5,   7,   6},
/*  intermediate faces */
{   0,   8, 0xC,   4}, {   0,   8, 0xA,   2}, { 0xA,   2,   6, 0xE}, { 0xE,   6,   4, 0xC},
{   1,   9, 0xB,   3}, { 0xB,   3,   7, 0xF}, {   7, 0xF, 0xD,   5}, { 0xD,   5,   1,   9},
{ 0xC, 0xD,   5,   4}, {   8,   9,   1,   0}, { 0xA, 0xB,   3,   2}, { 0xE, 0xF,   7,   6}
  };

  int visible[16];  /*  visibility flag for points */
  int n, i, k; /*  Loop counter. */

  /*  Move each point of the hypercube to its final position */
  for (n = 0; n < 16; n++)
  {
    visible[n] = 1;

    if ((sideVisible != NULL) && (dimension == 4))
    {
      for(i = eM4dAxisX; i < eM4dDimNum; i++)
      {
        visible[n] &= ((points[n].c[i] < 0) && (sideVisible[i][0] == 0)) ? 0 : 1;
        visible[n] &= ((points[n].c[i] > 0) && (sideVisible[i][1] == 0)) ? 0 : 1;
      }
    }

    /*  Dirty hack to get rid of Z-fighting between top cube and act. object */
    points[n] = m4dMultiplySV(1.01, points[n]);

    points[n] = m4dMultiplyMV(orientation, points[n]);

    visible[n] &= (dimension == 3) && (points[n].c[eM4dAxisW] < 0) ? 0 : 1;

    points[n] = m4dAddVectors(points[n], center);
  }

  /*  For each facet and */
  for (i = 0; i < 24; i++)
  {
    int sideVisible[4];
    int visiblePointNum = 0;
    int facetVisible = 0;

    tM4dVector pointlist[4];

    for (k = 0; k < 4; k++)
    {
      visiblePointNum += visible[faces[i][k]] ? 1 : 0;

      pointlist[k] = points[faces[i][k]];

      sideVisible[k] = visible[faces[i][k]];
    }

    facetVisible = (dimension == 3)
                   ? visiblePointNum == 4
                   : visiblePointNum > 0;

    if (facetVisible)
    {
      g4dDrawPoly(pointlist, color, wireMode, fill, sideVisible);
    }
  }
}
