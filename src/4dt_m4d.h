/**
 * \file  4dt_m4d.h
 * \brief Header for 4 dimensional math library.
 */

#ifndef _4DT_M4D_H_
#define _4DT_M4D_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/** Enums for coordinate axices */
typedef enum
{
  eM4dAxisX  = 0,
  eM4dAxisY  = 1,
  eM4dAxisZ  = 2,
  eM4dAxisW  = 3,
  eM4dDimNum = 4
} eM4dAxis;

/** 4D vector container */
typedef struct
{
  double c[eM4dDimNum];
} tM4dVector;

/** 4D matrix container */
typedef struct
{
  double c[eM4dDimNum][eM4dDimNum];
} tM4dMatrix;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern tM4dVector m4dNullVector();
extern tM4dVector m4dUnitVector(eM4dAxis axis);
extern tM4dVector m4dVector(double x, double y, double z, double w);

extern tM4dMatrix m4dNullMatrix();
extern tM4dMatrix m4dUnitMatrix();
extern tM4dMatrix m4dRotMatrix(eM4dAxis axis1, eM4dAxis axis2, double angle);

extern tM4dVector m4dAddVectors(tM4dVector vector1, tM4dVector vector2);
extern tM4dVector m4dMultiplySV(double scalar, tM4dVector vector);
extern tM4dVector m4dMultiplyMV(tM4dMatrix matrix, tM4dVector vector);
extern tM4dMatrix m4dMultiplyMM(tM4dMatrix matrixL, tM4dMatrix matrixR);

extern void m4dPrintVector(tM4dVector vector);
extern void m4dPrintMatrix(tM4dMatrix matrix);

#endif /* _4DT_M4D_H_ */
