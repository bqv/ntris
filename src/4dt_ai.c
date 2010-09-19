/**
 *  \file 4dt_ai.c
 *  \brief This file is the Computer gamer's logic.
 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>

#include "4dt_main.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_ai.h"

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Axices belongs to the turns around 1..4 axis */
static const char aiTurnAxices[4][2] = {{0, 1},{1, 2},{2, 0},{0, 3}};

/*------------------------------------------------------------------------------
   VARIABLES
------------------------------------------------------------------------------*/

/** flag for auto gamer */
static int aiAutoGamerON = 0;

/** Time step for autoplayer in msec */
static int aiTimeStep = 150;

/** Array of the number of turns by axises needed to the best situation. */
static int aiNeededTurns[4];

/** Array contains the CenterOf Gravity of each situation. */
static double aiCoG[4 * 4 * 4 * 4];

/** Array contains the number of turns for each situation. */
static int aiTurns[4 * 4 * 4 * 4];

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static int aiFindBestSolution(void);
static double aiProcessSitu(void);
static int aiSearchBestSitu(void);
static void aiDoStep(void);
static int aiTimer(int interval, void *param);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** Get function for auto player enabled flag*/
int aiIsActive(void)
{
  return(aiAutoGamerON);
}

void aiSetActive(int active)
{
  int timerMustStarted = (active && !aiAutoGamerON);

  aiAutoGamerON = active;

  if (timerMustStarted)
  {
    setTimerCallback(aiTimeStep, aiTimer, NULL);
  }
}

/** Timer function for Autoplayer. */
static int aiTimer(int interval, void *param)
{
  if (aiAutoGamerON)
  {
    if (engGE.gameOver == 0)
    {
      aiDoStep();
      refresh();
      return(aiTimeStep);
    }
    else
    {
      aiAutoGamerON = 0;
      return(0);
    }
  }
  else
  {
    return(0);
  }
}

/** Trigger the AI to make a turn. */
static void aiDoStep(void)
{
  // Local variables:
  char stepMade = 0; // inditcator of turn already made;
  int i;   // loop counters;

  static int solidnum = -1;

  // If got a new solid,
  if (solidnum != engGE.solidnum)
  {
    // find the best situation (num of turns) to drop it.
    aiFindBestSolution();
    solidnum = engGE.solidnum;
  }
  else
  { // else
    // For each axis,
    for (i = 0; i < 4; i++)
    {
      // if turn needed around and
      // not yet made any turn, then
      if (   (aiNeededTurns[i] > 0)
          && (!stepMade)
          && (!engGE.lock))
      {
        aiTimeStep = 200;

        // turn around the axis,
        engTurn(aiTurnAxices[i][0],
                aiTurnAxices[i][1]);
        // decrease the turns needed, and
        aiNeededTurns[i]--;
        // indicate it.
        stepMade = 1;
      }
    }
    // If no turn made, so no more turn needed, then
    if ((!stepMade) && (!engGE.lock))
    {
      aiTimeStep = 100;

      // lower the solid. }
      engLowerSolid();
    }
  }
}

/** Finds the best situation from all turn variation
 *  (the most effective one with fewest turn).
 *  \return id of optimal turn variation */
static int aiFindBestSolution(void)
{
  // Local variables:
  int i, j, n;             // loop counter;
  int x[4];                // loop counters;
  int bestSitu;            // number of the best situation
  tEngGame backup_ge; // backup game engine;
  double pos;

  // For each turn number variation:
  for (x[3] = 0; x[3] < 4; x[3]++)
    for (x[1] = 0; x[1] < 4; x[1]++)
      for (x[2] = 0; x[2] < 4; x[2]++)
        for (x[0] = 0; x[0] < 4; x[0]++)
  {
    // Back up the actual situation.
    backup_ge = engGE;

    engGE.animation.enable = 0;

    for (j = 0; j < 4; j++)
    {
      // Turn x[j] times around j.th axle.
      for (i = 0; i < x[j]; i++)
      {
        engTurn(aiTurnAxices[j][0],
                aiTurnAxices[j][1]);
      }
    }

    do
    {
      // store the actual pos
      pos = engGE.object.pos.c[eM4dAxisW];

      // lower the solid.
      engLowerSolid();

    // While not reached the floor
    }
    while (engGE.object.pos.c[eM4dAxisW] < pos);

    // Number of the situation.
    n = x[0]*64 + x[1]*16 + x[2]*4 + x[3];

    // Calculate Cog of the situation.
    aiCoG[n] = aiProcessSitu();

    // Calculate number of turns made.
    aiTurns[n] = x[0] + x[1] + x[2] + x[3];
    // Restore the actual situation.
    engGE = backup_ge;

  }

  // Return with the best of situations.
  bestSitu = aiSearchBestSitu();

  // Fill the array of the required steps.
  aiNeededTurns[3] = bestSitu % 4;
  aiNeededTurns[2] = bestSitu / 4 % 4;
  aiNeededTurns[1] = bestSitu / 16 % 4;
  aiNeededTurns[0] = bestSitu / 64 % 4;

  return bestSitu;

}  // End of function

/** Calculate Center of gravity of the game space with landed object.
 *  \return height (on 4th axis) of CoG */
static double aiProcessSitu(void)
{
  // Loop counters for axises.
  int x, y, z, l;
  // Center of gravity on l axis, sum of full cells.
  int cog, sum;

  cog = 0;
  sum = 0;

  // For each cell of gamespace,
  for (l = 0; l < SPACELENGTH; l++)
    for (x = 0; x < 2; x++)
      for (y = 0; y < 2; y++)
        for (z = 0; z < 2; z++)
  {
    // if the cell is full,
    if (engGetSpaceCell(l,x,y,z))
    {
      // inrease sum and
      sum += 1;
      // add the position to Cog.
      cog += l;
    }
  }

  // 'Normalise' CoG.
  if (sum == 0)
  {
    return 0.0;
  }
  else
  {
    return ( (double)cog / sum);
  }

}  // End of function.

/** Search the best situation.
 *  \return ID of optimal situation */
static int aiSearchBestSitu(void)
{
  // Local variables:
  int i,       // loop counter;
      cntCog,  // counter for;
      cntTurn; // counter for turns;
  int bestSitusCog[4 * 4 * 4 * 4];  // best situation container array;
  int bestSitusTurn[4 * 4 * 4 * 4]; // best situation container array;

  // lowest value of Cog;
  double lowerCog = SPACELENGTH;
  // lowest value of turn numbers.
  double fewerTurn = 4 * 4 * 4 * 4;

  // Find the minimal CoG result.

  // For each situation
  for (i = 0; i < 4 * 4 * 4 * 4; i++)
  {
    // If actual CoG lower then lowest,
    if (aiCoG[i] < lowerCog)
    {
      // the actual is the lowest.
      lowerCog = aiCoG[i];
    }
  }

  // No best situation found.
  cntCog = 0;

  // For each situation
  for (i = 0; i < 4 * 4 * 4 * 4; i++)
  {
    // If the situations cog is the best
    if (aiCoG[i] == lowerCog)
    {
      return i;

      // the situ. is one of the best situs.
      bestSitusCog[cntCog] = i;
      // Increase counter.
      cntCog++;
    }
  }


  // Find the one with minimal turning number
  // from best situations.

  // For each best situation
  for (i = 0; i < cntCog; i++)
  {
    // if actual situ. turns num. fewer then lowest,
    if (aiTurns[bestSitusCog[i]] < fewerTurn)
    {
      // the actual situs's turn num is the fewest.
      fewerTurn = aiTurns[bestSitusCog[i]];
    }
  }

  // No best situation found regarding turn numbers.
  cntTurn = 0;

  // For the best situations
  for (i = 0; i < cntCog; i++)
  {
    // if the turn number match with the best,
    if (aiTurns[bestSitusCog[i]] == fewerTurn)
    {
      return i;
      // the situation is best.
      bestSitusTurn[cntTurn] = i;
      // Increase counter.
      cntTurn++;
    }
  }

  // Select one from the best situations and return
  return bestSitusTurn[rand()*cntTurn/RAND_MAX];

}  // End of function.
