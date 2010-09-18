/**
 * \file  4dt_m3d.h
 * \brief Header for 3D math library.
 */

#ifndef _4DT_M3D_H_
#define _4DT_M3D_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/


typedef enum
{
  eM3dAxisX  = 0,
  eM3dAxisY  = 1,
  eM3dAxisZ  = 2,
  eM3dDimNum = 3
} eM3dAxis;


/** 3D vector container */
typedef struct
{
  double c[eM3dDimNum];
} tM3dVector;


/** 4D matrix container */
typedef struct
{
  double c[eM3dDimNum][eM3dDimNum];
} tM3dMatrix;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern tM3dVector m3dVector(double x, double y, double z);
extern tM3dVector m3dCalcNormal(tM3dVector v1, tM3dVector v2);
extern tM3dVector m3dSub(tM3dVector target, tM3dVector source);
extern tM3dVector m3dAdd(tM3dVector vector1, tM3dVector vector2);
extern double     m3dMultiplyVV(tM3dVector vector1, tM3dVector vector2);
extern tM3dVector m3dMultiplySV(double scalar, tM3dVector vector);
extern double     m3dAbs(tM3dVector vector);
extern tM3dVector m3dCrossProduct(tM3dVector vector1, tM3dVector vector2);
extern tM3dVector m3dNormalise(tM3dVector vector);
extern tM3dVector m3dInterpolate(tM3dVector vector1, tM3dVector vector2,
                                 double factor);
extern tM3dMatrix m3dTransformMatrixV(tM3dVector base);
extern tM3dMatrix m3dMatrixV3(tM3dVector v[3]);


#endif /* 4DT_M3D_H_ */
