/**
 * \file  4dt_main.h
 * \brief Header for main modul.
 */

#ifndef _4DT_MAIN_H_
#define _4DT_MAIN_H_

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

typedef int (*tTimerCallback)(int interval, void *param);

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern int *setTimerCallback(int time,
                             tTimerCallback callback,
                             void *param);
extern void clearTimerCallback(int *id);

#endif /* _4DT_MAIN_H_ */
