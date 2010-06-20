/**
 * \file  4dt_scn.h
 * \brief Header for modul for handling game scene
 */

#ifndef _4DT_SCN_H_
#define _4DT_SCN_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern int scnAxle;

extern void scnInit(void);
extern void scnDisplay(void);

extern void scnSetEnableHypercubeDraw(int enable);
extern int scnGetEnableHypercubeDraw(void);

#endif /* _4DT_SCN_H_ */
