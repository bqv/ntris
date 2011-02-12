/**
 * \file  4dt_mou.h
 * \brief Header for module of mouse event handlers
 */

#ifndef _4DT_MOU_H_
#define _4DT_MOU_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/


extern void mouResize(int width, int height);
extern void mouMove(int x, int y, tEngGame *pEngGame);
extern void mouDown(int x, int y);
extern void mouUp(int x, int y, tEngGame *pEngGame);

#endif
