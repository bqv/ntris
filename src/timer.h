/**
 * \file  main.h
 * \brief Header for main modul.
 */

#ifndef _MAIN_H_
#define _MAIN_H_

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

#endif /* _MAIN_H_ */
