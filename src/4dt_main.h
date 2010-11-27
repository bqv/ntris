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

/*------------------------------------------------------------------------------
   DECLARATIONS
------------------------------------------------------------------------------*/

extern int *setTimerCallback(int time,
                             int (*callback)(int interval, void *param),
                             void *param);
extern void refresh(void);
extern void clearTimerCallback(int *id);
extern void* getSDLScreen(void);

#endif /* _4DT_MAIN_H_ */
