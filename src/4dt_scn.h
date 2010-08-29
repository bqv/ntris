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
// todo: remove
extern int scnStereoEnable;

extern void scnInit(void);
extern void scnDisplay(void);

extern void scnSetStereoMode(int enable);
extern int  scnGetStereoMode(void);

extern void scnSetEnableHypercubeDraw(int enable);
extern int  scnGetEnableHypercubeDraw(void);

extern void scnSetEnableSeparateBlockDraw(int enable);
extern int  scnGetEnableSeparateBlockDraw(void);

extern void scnSetEnableGridDraw(int enable);
extern int  scnGetEnableGridDraw(void);

#endif /* _4DT_SCN_H_ */
