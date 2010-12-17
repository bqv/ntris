/**
 * \file  4dt_timer.c
 * \brief Wrapper for timers.
 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <SDL/SDL.h>

#include "4dt_timer.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/
/*
--------------------------------------------------------------------------------
   PROTOTYPES
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/

/** Sets a timer to call back the function passed after given time (msec) */
int *setTimerCallback(int time,
                      int (*callback)(int interval, void *param),
                      void *param)
{
  return(SDL_AddTimer(time, callback, param));
}

/** Remove the previously set timer */
void clearTimerCallback(int *id)
{
  SDL_RemoveTimer(id);

  return;
}
