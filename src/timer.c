/**
 * \file  timer.c
 * \brief Wrapper for timers.
 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <SDL/SDL.h>

#include "timer.h"

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
                      tTimerCallback callback,
                      void *param)
{
    return((int *)SDL_AddTimer(time, (SDL_NewTimerCallback)callback, param));
}

/** Remove the previously set timer */
void clearTimerCallback(int *id)
{
    SDL_RemoveTimer((SDL_TimerID)id);

    return;
}
