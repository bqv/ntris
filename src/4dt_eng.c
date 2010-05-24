/**
 * \file  4dt_eng.c
 * \brief This file is the game engine of the 4D Tetris.
 *
 *  Game space and element storage and handling.
 */
 
/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>

#include "4dt_eng.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** number of type of solids */
#define SOLIDTYPES (6)

/** number of difficulty levels */
#define DIFFLEVELS (3)

/*------------------------------------------------------------------------------
// CONSTANTS
------------------------------------------------------------------------------*/


/** time ellapsed while the the solid steps one level down in sec; */
static double timestep = 1.5;

/** defined solids */
static TSolid solids[SOLIDTYPES] =
/* 1 cube */  {0x0001, // 00 00 00 00  00 00 00 01
/* 2 cube */   0x0003, // 00 00 00 00  00 00 00 11
/* 3 cube L */ 0x0007, // 00 00 00 00  00 00 01 11
/* 4 cube */   0x000f, // 00 00 00 00  00 00 11 11
/* 4 cube */   0x0027, // 00 00 00 00  00 10 11 11 
/* 4 cube */   0x0017};// 00 00 00 00  00 01 01 11 

/** probabilities of solids in different
    difficulty levels (easy, medium, hard) */
static int probs[DIFFLEVELS][SOLIDTYPES] =
{{10, 5, 5, 1, 1, 1},
 {5, 3, 2, 1, 1, 1},
 {2, 1, 1, 1, 1, 1}};
  
/** Score points for filling a level in different difficulty levels */
static int scoreStep[DIFFLEVELS] = { 100, 200, 400};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Game engine container */
t_game_Engine ge;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

extern t_game_Engine copyGameEngine(t_game_Engine in_game_Engine);
extern bool lowerSolid(void);
extern void NewSolid(void);
extern bool checkOverlap(void);
extern void killFullLevels(void);
extern bool turn(char ax1, char ax2);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/


/** triggers the autoplayer engine */
void engTrigger(float time /**< ellapsed time since program started in sec */)
{
  // Time storage for lower down the solid
  // initialised with ellapsed time since program started.
  static double timeLower = 0.0;

  // If time ellapsed since last storage larger the time of step down,
  if (time - timeLower > timestep) {
    // lower the solid and
    lowerSolid();
    // store the actual time.
    timeLower = time;
  }
}

/** duplicates the game engine */
t_game_Engine copyGameEngine(t_game_Engine in_game_Engine)
{
  // Local variables:
  t_game_Engine result; // duplicated game engine;
  int i;                // loop counter.
  
  // Copy the input game engine to the result one.
  result = in_game_Engine;
  
  // Copy the gamespace to the result.
  for (i = 0; i < SPACELENGTH; i++)
  {
    result.space[i] = in_game_Engine.space[i];
  }
  
  // return with the result gamespace
  return result;
}

/** initialize the game variables */
void initGame(void)
{
   char t; // Loop counter.

   // for the every part of the space
   for (t = 0; t < SPACELENGTH; t++)
   {
      ge.space[t] = 0x00;
   }

   // initialise the number of solids dropped
   ge.solidnum = 0;

   // get new solid
   NewSolid();
   
   // set options
   ge.game_opts.diff = 2;
   
   // init score value
   ge.score = 0;
}

/** get a new random solid */
void NewSolid(void)
{
   // probability
   int prob; 
   // sum of probabilities
   int sum = 0;  
   // loop counter
   int i;

   // for every solid type
   for (i = 0; i < SOLIDTYPES; i++)
   {
      // summarize the probability
      sum += probs[ge.game_opts.diff][i];
   }
   
   // initialize loop counter
   i = 0; 
   
   // get a random probability
   prob = (long int) (sum * (rand() / (RAND_MAX + 1.0) ));
   
   int prb = probs[ge.game_opts.diff][i];
   
   while (!(prob < prb))
   { 
     i++;
     prb += probs[ge.game_opts.diff][i];
   }

   ge.solid = solids[i];

   // position the new solid to the
   // top 2 level of the space
   ge.pos = SPACELENGTH - 2;
   
   // set the indicator of new solid true
   ge.isnewsolid = 1;

   // increase the number of the solid
   ge.solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
bool checkOverlap(void)
{
   return !!( ( (TSolid)ge.space[ge.pos+1] << 8 | ge.space[ge.pos]) & ge.solid); 

}//end of checkOverlap

/** deletes the full levels */
void killFullLevels(void)
{
   // loop counter
   int t, tn;

   // for every level
   for(t = 0; t < SPACELENGTH; t++)
   {
      // if full level found
      if (ge.space[t] == 0xff) 
      {
         // increase score
         ge.score += scoreStep[ge.game_opts.diff];
                   
         // step down every higher level
         for (tn = t+1; tn <= SPACELENGTH; tn++)
         {
            // get the next level or 0 on the top level
            ge.space[tn-1] = ge.space[tn] * 
                         (tn < SPACELENGTH);

         } // end of every level

      } // end of if full

   } // end of every level

} //end of checkFullLevels

/** lower the solid with one level
   \return false if invalid (end of game) */
bool lowerSolid(void)
{
   // set the new solid indicator false
   ge.isnewsolid = 0;

   // solid reached the floor flag
   bool onFloor = (ge.pos <= 0) || 
                  ( (ge.pos-- || 1) &&
                    checkOverlap() && 
                    (ge.pos++ || 1) );

   // if reached the floor,
   if (onFloor)
   {
      // put the solid to the space
      ge.space[ge.pos]   |= ge.solid;
      ge.space[ge.pos+1] |= ge.solid >> 8;
               
      // delete the full levels
      killFullLevels();
      // get new solid
      NewSolid();

      // check new solid already overlapped
      return (!checkOverlap());
   }
}

/** turns the solid from axis 1 to axis 2
   \return indicator of turn availability */
bool turn(char ax1, char ax2)
{
   // loop cnt, temp var
   int n, n2;

   // remove the solid to a temp solid
   TSolid tempSolid = ge.solid;
   ge.solid = 0x0000;

   // turn it

   // for every cell
   for (n = 0; n < 16; n++)
   {
     // the bits of the number of the element are 
     // the coords of the element - so the num = pos vector
     
     // turn the vector (replace the two bit specified by axis num,
     //    and negate the second)
     n2 = n & (~(0x01 << ax2)) 
            & (~(0x01 << ax1)) 
            | (   ((n >> ax1) & 0x01) << ax2) 
            | ( (!((n >> ax2) & 0x01)) << ax1);

      // get the element to the new turned place
      ge.solid |= ((tempSolid >> n) & 0x01) << n2;
   }

   // put to the lower level of the solid
   // if lower level empty then
   if (ge.solid && 0x00ff == 0x00)
      // move from the higher level to the lower
      ge.solid = ge.solid >> 8;

   // if overlapped, invalid turn
   // get back the original
   return !(checkOverlap() && (ge.solid = tempSolid)); 
 
}


