/**
 * \file  4dt_gtxt.h
 * \brief Header of text drawing modul.
 */

#ifndef _4DT_GTXT_H_
#define _4DT_GTXT_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern  void g3dRenderString(double x, double y,
                             float color[4],
                             char *string);

extern void g3dRenderText(double x, double y,
                          float color[4],
                          char **strings,
                          int lineNum,
                          double lineSpace);

extern void gtxtResize(int width, int height);

#endif /* _4DT_GTXT_H_ */
