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

extern void g4dDrawTriangle(float points[3][4],
                            float colors[3][4],
                            int mode);


#endif /* _4DT_G4D_H_ */
