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

typedef enum
{
  eScnViewMono,
  eScnViewStereogram,
  eScnViewAnaglyph,
  eScnViewModeNum
}
tScnViewMode;

typedef struct
{
  /** Flag indicates if hypercube should be drawn or only the "top side" cube */
  int enableHypercubeDraw;
  /** Flag indicates if block should be drawn separated */
  int enableSeparateBlockDraw;
  /** Flag indicates if grid has to be drawn */
  int enableGridDraw;
  /** selected axle for rotation */
  int axle;
  /** temporary switch for stereo view */
  tScnViewMode viewMode;
}
tScnSet;

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void scnInit(void);
extern tScnSet scnGetDefaultSet(void);
extern void scnDisplay(tEngGame *pEngGame, tScnSet *pScnSet);

extern void scnSetViewMode(tScnViewMode mode, tScnSet *pScnSet);
extern tScnViewMode scnGetViewMode(tScnSet *pScnSet);

extern void scnSetEnableHypercubeDraw(int enable, tScnSet *pScnSet);
extern int  scnGetEnableHypercubeDraw(tScnSet *pScnSet);

extern void scnSetEnableSeparateBlockDraw(int enable, tScnSet *pScnSet);
extern int  scnGetEnableSeparateBlockDraw(tScnSet *pScnSet);

extern void scnSetEnableGridDraw(int enable, tScnSet *pScnSet);
extern int  scnGetEnableGridDraw(tScnSet *pScnSet);

#endif /* _4DT_SCN_H_ */
