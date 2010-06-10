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

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void g4dCalibrate(double w_maximum);

extern void g4dDraw4DCube(tM4dVector center,
                          tM4dMatrix orientation,
                          float color[4],
                          int dimension,
                          int mode);

extern void g4dDrawLine(float point0[4],
                        float point1[4],
                        float color0[4],
                        float color1[4],
                        float linewidth);

#endif /* _4DT_G4D_H_ */
