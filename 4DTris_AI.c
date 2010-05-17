/*

 Project: 4D-TRIS
 Author: Simon, Laszlo

 Short Description:
 This file is the Computer gamer's logic.

 Change Log:
 1.2 - Fixed - simonl - 2008 jan 13
       Code fixed and file renamed.

 1.1 - Initial revision - simonl - 2008 jan 08

 ToDo:
 - Improve AI with considering overlap:
    |    | |    | |    |
    |**  | | ** | |  **|
    |XX X| |XX X| |XX X|
   Now these are equivalent.

 Known Bugs:
 Dont find the best solution when 1 block only and 0,0,0,0 coord empty.

 */

/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>

#include <stdlib.h>
#include "4DTris_engine.h"


/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

// Axises belongs to the turns around 1..4 axis
char turnAxises[4][2] = {{0, 1},{1, 2},{2, 0},{0, 3}};

/*------------------------------------------------------------------------------
   VARIABLES
------------------------------------------------------------------------------*/

// Struct of game engine.
extern t_game_Engine ge;

// Array of the number of turns by axises needed to the best situation.
int neededTurns[4];

// Array contains the CenterOf Gravity of each situation.
double cog[4 * 4 * 4 * 4];

// Array contains the number of turns for each situation.
int turns[4 * 4 * 4 * 4];

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

// Trigger the AI to make a turn.
void dostep(void);

// Finds the best situation from all turn variation
// (the most effective one with fewest turn).
int findBestSolution(void);

// Calculate Cog of the situation.
double ProcessSitu(void);

// Search the best situation.
int SearchBestSitu(void);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

// Trigger the AI to make a turn.
void dostep(void)
{
  // Local variables:
  char stepMade = 0; // inditcator of turn already made;
  int i, ax1, ax2;   // loop counters.

  static int solidnum = ge.solidnum;

  // If got a new solid,
  if (solidnum != ge.solidnum)
  {
    // find the best situation (num of turns) to drop it.
    findBestSolution();
    solidnum = ge.solidnum;
  }
  else
  {
    // For each axis,
    for (i = 0; i < 4; i++)
    {
      // if turn needed around and
      // not yet made any turn, then
      if ((neededTurns[i] > 0) &&
          !stepMade)
      {
        // turn around the axis,
        turn(turnAxises[i][0],
             turnAxises[i][1]);
        // decrease the turns needed, and
        neededTurns[i]--;
        // indicate it.
        stepMade = 1;
      }
    }
    // If no turn made, so no more turn needed, then
    if (!stepMade)
    {
      // lower the solid.
      lowerSolid();
    }
  }
}

// Finds the best situation from all turn variation
// (the most effective one with fewest turn).
int findBestSolution(void)
{
  // Local variables:
  int i, j, n;             // loop counter;
  int x[4];                // loop counters;
  int bestSitu;            // number of the best situation
  t_game_Engine backup_ge; // backup game engine;

int pos;

  // For each turn number variation:
  for (x[3] = 0; x[3] < 4; x[3]++)
    for (x[1] = 0; x[1] < 4; x[1]++)
      for (x[2] = 0; x[2] < 4; x[2]++)
        for (x[0] = 0; x[0] < 4; x[0]++)
  {
    // Back up the actual situation.
    backup_ge = copyGameEngine(ge);

    // store the actual pos
    pos = ge.pos;

    for (j = 0; j < 4; j++)
    {
      // Turn x[j] times around j.th axle.
      for (i = 0; i < x[j]; i++)
      {
        turn(turnAxises[j][0],
             turnAxises[j][1]);
      }
    }

    // While not reached the floor
    while (ge.pos <= pos)
    {
      // store the actual pos
      pos = ge.pos;

      // lower the solid.
      lowerSolid();
    }

    // Number of the situation.
    n = x[0]*64 + x[1]*16 + x[2]*4 + x[3];

    // Calculate Cog of the situation.
    cog[n] = ProcessSitu();

    // Calculate number of turns made.
    turns[n] = x[0] + x[1] + x[2] + x[3];
    // Restore the actual situation.
    ge = copyGameEngine(backup_ge);

  }

  // Return with the best of situations.
  bestSitu = SearchBestSitu();

  // Fill the array of the required steps.
  neededTurns[3] = bestSitu % 4;
  neededTurns[2] = bestSitu / 4 % 4;
  neededTurns[1] = bestSitu / 16 % 4;
  neededTurns[0] = bestSitu / 64 % 4;

  return bestSitu;

}  // End of function

// Calculate Cog of the situation.
double ProcessSitu(void)
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
    if (getSpaceCell(l,x,y,z))
    {
      // inrease summ and
      sum += 1;
      // add the position to Cog.
      cog += l;
    }
  }

  // 'Normalise' CoG.
  return ( (double)cog / sum);

}  // End of function.

// Search the best situation.
int SearchBestSitu(void)
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
    if (cog[i] < lowerCog)
    {
      // the actual is the lowest.
      lowerCog = cog[i];
    }
  }

  // No best situation found.
  cntCog = 0;

  // For each situation
  for (i = 0; i < 4 * 4 * 4 * 4; i++)
  {
    // If the situations cog is the best
    if (cog[i] == lowerCog)
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
    if (turns[bestSitusCog[i]] < fewerTurn)
    {
      // the actual situs's turn num is the fewest.
      fewerTurn = turns[bestSitusCog[i]];
    }
  }

  // No best situation found regarding turn numbers.
  cntTurn = 0;

  // For the best situations
  for (i = 0; i < cntCog; i++)
  {
    // if the turn number match with the best,
    if (turns[bestSitusCog[i]] == fewerTurn)
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
