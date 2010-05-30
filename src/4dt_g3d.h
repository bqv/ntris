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

extern double g3dAngleX;
extern double g3dAngleZ;
extern double g3dDAngle;
extern int g3dAutoRotationEnabled;


extern void g3dInit(void);
extern void g3dDrawPoly(float point[4][4],
                        float color[4],
                        int enableWire);
extern void g3dRenderString(double x, double y,
                           float color[4],
                           char *string);
void g3dRenderText(double x, double y,
                   float color[4],
                   char **strings,
                   int lineNum,
                   double lineSpace);
extern void g3dDrawRectangle(float x0, float y0, float x1, float y1,
                      float color1[4], float color2[4]);
extern void g3dBeginDraw(void);
extern void g3dEndDraw(void);
extern void g3dResize(int width, int height);
extern void g3dResetViewport(void);
extern int  m3dRotateViewport(void);


#endif /* _4DT_G3D_H_ */
