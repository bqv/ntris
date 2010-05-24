/**
 *  \file  4dt_g3d.h
 *  \brief Header for 3D OpenGL scene rendering modul.
 */

#ifndef _4DT_G3D_H_
#define _4DT_G3D_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern double angleX;
extern double angleZ;
extern double dangle;


extern void g3dInit(int argc, char *argv[]);
extern void g3dDrawPoly(float point[4][4],
                        float color[4],
                        int enableWire);
extern void g3dRenderString(double x, double y, double z,
                           float color[4],
                           char *string);
extern void g3dBeginPreDraw(void);
extern void g3dBeginDraw(void);
extern void g3dEndDraw(void);

#endif /* _4DT_G3D_H_ */
