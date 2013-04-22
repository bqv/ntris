/**
 * \file  gtxt.h
 * \brief Header of text drawing modul.
 */

#ifndef _GTXT_H_
#define _GTXT_H_

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

#endif /* _GTXT_H_ */
