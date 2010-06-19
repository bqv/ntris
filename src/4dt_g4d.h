/**
 * \file  4dt_g4d.h
 * \brief Header for 4 dimensional drawing module.
 */
 
#ifndef _4DT_G4D_H_
#define _4DT_G4D_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

typedef enum
{
  eG4d1PointProjection,
  eG4d2PointProjection,
}
tG4dViewType;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void g4dInit(double w_maximum);
extern void g4dReset(void);
extern void g4dSetViewType(tG4dViewType viewType);

extern void g4dSwitchAutoRotation(int enable);
extern int  g4dAutoRotateViewport(void);
extern void g4dRotateViewport(eM4dAxis axis1, eM4dAxis axis2, double angle);

extern void g4dDraw4DCube(tM4dVector center,
                          tM4dMatrix orientation,
                          float color[4],
                          int dimension,
                          int mode);

extern void g4dDrawLine(tM4dVector point0,
                        tM4dVector point1,
                        float color0[4],
                        float color1[4],
                        float linewidth);

#endif /* _4DT_G4D_H_ */
