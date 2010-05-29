/**
 * \file  4dt_m4d.c
 * \brief 4 dimensional math library.
 */
 
/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Creates a null vector. */
tM4dVector m4dNullVector()
{
  tM4dVector null = {{0.0, 0.0, 0.0, 0.0}};

  return null;
}

/** Creates a unit vector oriented to the given coordinate axis. */
tM4dVector m4dUnitVector(eM4dAxis axis)
{
  tM4dVector unit = {{0.0, 0.0, 0.0, 0.0}};

  unit.c[axis] = 1.0;

  return unit;
}

/** Creates a vector with the given coordinates */
tM4dVector m4dVector(double x, double y, double z, double w)
{
  tM4dVector vector;

  vector.c[eM4dAxisX] = x;
  vector.c[eM4dAxisY] = y;
  vector.c[eM4dAxisZ] = z;
  vector.c[eM4dAxisW] = w;

  return vector;
}


/** Creates a null matrix. */
tM4dMatrix m4dNullMatrix()
{
  tM4dMatrix null = { {{0.0, 0.0, 0.0, 0.0},
                       {0.0, 0.0, 0.0, 0.0},
                       {0.0, 0.0, 0.0, 0.0},
                       {0.0, 0.0, 0.0, 0.0}} };

  return null;
}

/** Creates a unit matrix. */
tM4dMatrix m4dUnitMatrix()
{
  tM4dMatrix unit = { {{1.0, 0.0, 0.0, 0.0},
                       {0.0, 1.0, 0.0, 0.0},
                       {0.0, 0.0, 1.0, 0.0},
                       {0.0, 0.0, 0.0, 1.0}} };

  return unit;
}

/** Creates a rotation transformation matrix.
 *  Matrix rotates around the plane specified by the given coordinate axices
 *  by given angle.*/
tM4dMatrix m4dRotMatrix(eM4dAxis axis1, eM4dAxis axis2, double angle)
{
  tM4dMatrix rot = m4dUnitMatrix();

  rot.c[axis1][axis1] =  cos(angle);
  rot.c[axis1][axis2] = -sin(angle);
  rot.c[axis2][axis1] =  sin(angle);
  rot.c[axis2][axis2] =  cos(angle);

  return rot;
}

/** Multiplies the given matrix with the given vector. */
tM4dVector m4dMultiplyMV(tM4dMatrix matrix, tM4dVector vector)
{
  eM4dAxis row, col;
  tM4dVector result = m4dNullVector();

  for (row = eM4dAxisX; row < eM4dDimNum; row++)
  for (col = eM4dAxisX; col < eM4dDimNum; col++)
  {
    result.c[row] += vector.c[col] * matrix.c[row][col];
  }

  return result;
}

/** Multiplies two matrices */
tM4dMatrix m4dMultiplyMM(tM4dMatrix matrixL, tM4dMatrix matrixR)
{
  eM4dAxis row, col, n;
  tM4dMatrix result = m4dNullMatrix();

  for (row = eM4dAxisX; row < eM4dDimNum; row++)
  for (col = eM4dAxisX; col < eM4dDimNum; col++)
  for (n = eM4dAxisX; n < eM4dDimNum; n++)
  {
    result.c[row][col] += matrixL.c[row][n] * matrixR.c[n][col];
  }

  return result;
}

void m4dPrintVector(tM4dVector vector)
{
  eM4dAxis axis;

  printf("{");
  for (axis = eM4dAxisX; axis < eM4dDimNum; axis++)
  {
    printf("%f\t", vector.c[axis]);
  }
  printf("}\n");
}

void m4dPrintMatrix(tM4dMatrix matrix)
{
  eM4dAxis axis1, axis2;

  printf("{\n");
  for (axis1 = eM4dAxisX; axis1 < eM4dDimNum; axis1++)
  {
    printf("{");
    for (axis2 = eM4dAxisX; axis2 < eM4dDimNum; axis2++)
    {
      printf("%f\t", matrix.c[axis1][axis2]);
    }
    printf("}\n");
  }
  printf("}\n");
}

#ifdef TEST

void m4dPrintVector(tM4dVector vector)
{
  eM4dAxis axis;

  printf("{");
  for (axis = eM4dAxisX; axis < eM4dDimNum; axis++)
  {
    printf("%f\t", vector.c[axis]);
  }
  printf("}\n");
}

void m4dPrintMatrix(tM4dMatrix matrix)
{
  eM4dAxis axis1, axis2;

  printf("{\n");
  for (axis1 = eM4dAxisX; axis1 < eM4dDimNum; axis1++)
  {
    printf("{");
    for (axis2 = eM4dAxisX; axis2 < eM4dDimNum; axis2++)
    {
      printf("%f\t", matrix.c[axis1][axis2]);
    }
    printf("}\n");
  }
  printf("}\n");
}

int main(void)
{
  tM4dVector vector1, vector2;
  tM4dMatrix matrix1, matrix2;

  printf("\nm4dNullVector:\n");
  m4dPrintVector(m4dNullVector());

  printf("\nm4dUnitVector(eM4dAxisZ):\n");
  m4dPrintVector(m4dUnitVector(eM4dAxisZ));

  printf("\nm4dNullMatrix():\n");
  m4dPrintMatrix(m4dNullMatrix());

  printf("\nm4dUnitMatrix():\n");
  m4dPrintMatrix(m4dUnitMatrix());

  printf("\nm4dRotMatrix(eM4dAxisX, eM4dAxisZ, 3.1415 / 6):\n");
  m4dPrintMatrix(m4dRotMatrix(eM4dAxisX, eM4dAxisZ, 3.1415 / 6));

  printf("\nm4dMultiplyMV:\n");
  matrix1 = m4dRotMatrix(eM4dAxisX, eM4dAxisZ, 3.1415 / 6);
  vector1 = m4dUnitVector(eM4dAxisX);
  vector2 = m4dMultiplyMV(matrix1, vector1);
  m4dPrintMatrix(matrix1);
  printf(" X ");
  m4dPrintVector(vector1);
  printf(" = ");
  m4dPrintVector(vector2);
}

/* Embedded perl code to compile and run moduletest. execute: perl -x 4dt_m4d.c
#!perl
print `gcc -D TEST=1 -o 4dt_m4d 4dt_m4d.c -lm && ./4dt_m4d`;
# */
#endif
