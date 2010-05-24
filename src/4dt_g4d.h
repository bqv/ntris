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

extern void g4dDrawPoly(float points[4][4],
                        float color[4],
                        int enableWire);

extern void g4dDraw4DCube(double x, double y, double z, double l,
                          float color[4],
                          int dimension,
                          int enableWire);

#endif /* _4DT_G4D_H_ */
