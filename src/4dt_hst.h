/**
 *  \file 4dt_hst.h
 *  \brief Header for High score table handling.
 */
 
#ifndef _4DT_HST_H_
#define _4DT_HST_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern void hstGetScoreTab(char ***textTab, int scoreNum);
extern void hstAddScore(int score);
extern void hstInit(void);

#endif /* _4DT_HST_H_ */
