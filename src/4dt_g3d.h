/**
 *  \file  4dt_g3d.h
 *  \brief Header for 3D OpenGL scene rendering modul.
 */

#ifndef _4DT_G3D_H_
#define _4DT_G3D_H_

#ifndef _4DT_M3D_H_
#error "please include 4dt_m3d.h first"
#endif
/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void g3dInit(void);
extern void g3dDrawPoly(tM3dVector points[4],
                        float color[4],
                        int enableWire,
                        int sideVisible[4]);
extern void g3dDrawLine(tM3dVector point0,
                        tM3dVector point1,
                        float color0[4],
                        float color1[4],
                        float linewidth);
extern void g3dRenderString(double x, double y,
                           float color[4],
                           char *string);
extern void g3dRenderText(double x, double y,
                          float color[4],
                          char **strings,
                          int lineNum,
                          double lineSpace);
extern void g3dDrawRectangle(float x0, float y0, float x1, float y1,
                      float color1[4], float color2[4]);
extern void g3dBeginDraw(void);
extern void g3dEndDraw(void);
extern void g3dResize(int width, int height);

#endif /* _4DT_G3D_H_ */
