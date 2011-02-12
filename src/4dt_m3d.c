/**
 * \file  4dt_m3d.c
 * \brief 3D math library.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "4dt_m.h"
#include "4dt_m3d.h"

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

/** Calculates scalar multiplication for 2 vector */
double m3dMultiplyVV(tM3dVector vector1, tM3dVector vector2)
{
  double result;

  result =   vector1.c[0] * vector2.c[0]
           + vector1.c[1] * vector2.c[1]
           + vector1.c[2] * vector2.c[2];

  return(result);
}

/** Creates a vector with the given coordinates */
tM3dVector m3dVector(double x, double y, double z)
{
  tM3dVector vector;

  vector.c[eM3dAxisX] = x;
  vector.c[eM3dAxisY] = y;
  vector.c[eM3dAxisZ] = z;

  return(vector);
}

/** Creates a sphere from the center vector and radius */
tM3dSphere m3dSphere(tM3dVector o, double r)
{
  tM3dSphere sphere;

  sphere.o = o;
  sphere.r = r;

  return(sphere);
}

/** Creates a plane from a point and a normal vector */
tM3dPlane m3dPlane(tM3dVector point, tM3dVector normal)
{

  tM3dPlane plane;

  plane.point  = point;
  plane.normal = normal;

  return(plane);
}

/** Intersection points betwenn line and plane */
void m3dSectPlaneLine(tM3dPlane plane, tM3dLine line, tM3dVector *section)
{
  double n;

  tM3dVector orient = m3dSub(line.p1, line.p0);
  tM3dVector shift  = m3dSub(line.p0, plane.point);

  /* (shift + n * orient) * norm = 0 =>
   * n = - shift * norm / (orient * norm) */
  n = - m3dMultiplyVV(shift, plane.normal) / m3dMultiplyVV(orient, plane.normal);

  *section = m3dAdd(line.p0,m3dMultiplySV(n, orient));
}

/** Intersections points betwenn line and sphere */
void m3dSectSphereLine(tM3dSphere sphere,
                       tM3dLine line,
                       tM3dVector *section0,
                       tM3dVector *section1)
{
  double a, b, c, n0, n1;

  tM3dVector orient = m3dSub(line.p1, line.p0);
  tM3dVector shift  = m3dSub(line.p0, sphere.o);
  /* line shifted to get sphere center to origo */

  /* (shift + n * orient)^2 = r^2  =>
   * n^2 * orient^2 + n * 2 * orient * shift + shift^2 - r^2 = 0*/
  a = m3dMultiplyVV(orient, orient);
  b = 2.0 * m3dMultiplyVV(orient, shift);
  c = m3dMultiplyVV(shift, shift) - sphere.r*sphere.r;

  mSolveSqrEq(a, b, c, &n0, &n1);

  *section0 = m3dAdd(line.p0,m3dMultiplySV(n0, orient));
  *section1 = m3dAdd(line.p0,m3dMultiplySV(n1, orient));
}

/** Creates a line from the two given vector */
tM3dLine m3dLine(tM3dVector p0, tM3dVector p1)
{
  tM3dLine line;

  line.p0 = p0;
  line.p1 = p1;

  return(line);
}

/** Calculates the transformation matrix which rotates Z axis to base vector */
tM3dMatrix m3dTransformMatrixV(tM3dVector base)
{
  tM3dMatrix result;
  tM3dVector normals[3];

  normals[0] = m3dNormalise(base);

  normals[1] = m3dNormalise(m3dVector(base.c[1]/base.c[0], 1, 0));

  normals[2] = m3dCrossProduct(normals[0], normals[1]);

  result = m3dMatrixV3(normals);

  return(result);
}

/** Creates matrix from 3 vector */
tM3dMatrix m3dMatrixV3(tM3dVector v[3])
{
  tM3dMatrix result;
  eM3dAxis axis1, axis2;

  for (axis1 = eM3dAxisX; axis1 < eM3dDimNum; axis1++)
    for (axis2 = eM3dAxisX; axis2 < eM3dDimNum; axis2++)
  {
    result.c[axis1][axis2] = v[axis1].c[axis2];
  }

  return(result);
}

/** Subctract source vector from target vector */
tM3dVector m3dSub(tM3dVector target, tM3dVector source)
{
  tM3dVector result;
  eM3dAxis axis;

  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    result.c[axis] = target.c[axis] - source.c[axis];
  }

  return result;
}

/** Adds two vector */
tM3dVector m3dAdd(tM3dVector vector1, tM3dVector vector2)
{
  tM3dVector result;
  eM3dAxis axis;

  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    result.c[axis] = vector1.c[axis] + vector2.c[axis];
  }

  return result;
}


/** Scalar-vector multiplication */
tM3dVector m3dMultiplySV(double scalar, tM3dVector vector)
{
  eM3dAxis axis;
  tM3dVector result;

  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    result.c[axis] = scalar * vector.c[axis];
  }

  return(result);
}

/** Calculate length of the vector */
double m3dAbs(tM3dVector vector)
{
  double abs = 0;
  eM3dAxis axis;

  /* For each coordinate */
  for (axis = eM3dAxisX; axis < eM3dDimNum; axis++)
  {
    /* normalise the coordinate. */
    abs += vector.c[axis] * vector.c[axis];
  }

  abs = sqrt(abs);

  return(abs);
}

/** \brief Calculates the crossProduct of v1 and v2 vector where
    'n' the result vector. */
tM3dVector m3dCrossProduct(tM3dVector vector1, tM3dVector vector2)
{
  tM3dVector cross;

  /* Calculate crossproduct by coordinates. */
  cross.c[0] = vector1.c[1] * vector2.c[2] - vector2.c[1] * vector1.c[2];
  cross.c[1] = vector1.c[2] * vector2.c[0] - vector2.c[2] * vector1.c[0];
  cross.c[2] = vector1.c[0] * vector2.c[1] - vector2.c[0] * vector1.c[1];

  return (cross);
}

/** Normalise a vector. */
tM3dVector m3dNormalise(tM3dVector vector)
{
  /* Calculate length of the vector. */
  double length = m3dAbs(vector);

  /* normalise the coordinates. */
  vector = m3dMultiplySV(1/length, vector);

  return(vector);
}

/** Calculate normal vector of the plane of v1, v2 vector. */
tM3dVector m3dCalcNormal(tM3dVector v1, tM3dVector v2)
{
  tM3dVector normal;

  normal = m3dCrossProduct(v1, v2);

  m3dNormalise(normal);

  return(normal);
}

/** Interpolates between the two given vector */
tM3dVector m3dInterpolate(tM3dVector vector1, tM3dVector vector2,
                                 double factor)
{
  tM3dVector result;

  result = m3dAdd(m3dMultiplySV(1.0 - factor, vector1),
                  m3dMultiplySV(factor, vector2));

  return(result);
}

/** Returns the main orthogonal axis (longest coordinate) */
eM3dAxis m3dOrtho(tM3dVector vector)
{
  return(     (fabs(vector.c[0]) > fabs(vector.c[1]))
           && (fabs(vector.c[0]) > fabs(vector.c[2])) ? eM3dAxisX :
              (fabs(vector.c[1]) > fabs(vector.c[0]))
           && (fabs(vector.c[1]) > fabs(vector.c[2])) ? eM3dAxisY :
                                                        eM3dAxisZ );
}
