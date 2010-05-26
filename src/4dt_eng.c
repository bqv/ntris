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
#include <time.h>


#include "4dt_eng.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** number of type of solids */
#define SOLIDTYPES (6)

/*------------------------------------------------------------------------------
// CONSTANTS
------------------------------------------------------------------------------*/

/** defined solids */
static const TSolid engSolids[SOLIDTYPES] =
/* 1 cube */  {0x0001, // 00 00 00 00  00 00 00 01
/* 2 cube */   0x0003, // 00 00 00 00  00 00 00 11
/* 3 cube L */ 0x0007, // 00 00 00 00  00 00 01 11
/* 4 cube */   0x000f, // 00 00 00 00  00 00 11 11
/* 4 cube */   0x0027, // 00 00 00 00  00 10 11 11 
/* 4 cube */   0x0017};// 00 00 00 00  00 01 01 11 

/** probabilities of solids in different
    difficulty levels (easy, medium, hard) */
static const int engProbs[DIFFLEVELS][SOLIDTYPES] =
{{10, 5, 5, 1, 1, 1},
 {5, 3, 2, 1, 1, 1},
 {2, 1, 1, 1, 1, 1}};
  
/** Score points for filling a level in different difficulty levels */
static const int engScoreStep[DIFFLEVELS] = { 100, 200, 400};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Game engine container */
t_game_Engine engGE;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void engNewSolid(void);
static bool engCheckOverlap(void);
static void engKillFullLevels(void);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

/** duplicates the game engine */
t_game_Engine engCopyGameEngine(t_game_Engine in_game_Engine)
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

/** Reset game variables */
void engResetGame(void)
{
  int t; // Loop counter.

  // for the every part of the space
  for (t = 0; t < SPACELENGTH; t++)
  {
    engGE.space[t] = 0x00;
  }

  // initialise the number of solids dropped
  engGE.solidnum = 0;

  // get new solid
  engNewSolid();
   
  // init score value
  engGE.score = 0;

  engGE.gameOver = 0;
}


/** initialize the game variables */
void engInitGame(void)
{
  // initialize random generator
  srand(time(NULL));

  // set options
  engGE.game_opts.diff = 2;

  // reset parameters
  engResetGame();
}

/** get a new random solid */
static void engNewSolid(void)
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
      sum += engProbs[engGE.game_opts.diff][i];
   }
   
   // initialize loop counter
   i = 0; 
   
   // get a random probability
   prob = (long int) (sum * (rand() / (RAND_MAX + 1.0) ));
   
   int prb = engProbs[engGE.game_opts.diff][i];
   
   while (!(prob < prb))
   { 
     i++;
     prb += engProbs[engGE.game_opts.diff][i];
   }

   engGE.solid = engSolids[i];

   // position the new solid to the
   // top 2 level of the space
   engGE.pos = SPACELENGTH - 2;
   
   // set the indicator of new solid true
   engGE.isnewsolid = 1;

   // increase the number of the solid
   engGE.solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
static bool engCheckOverlap(void)
{
   return !!( ( (TSolid)engGE.space[engGE.pos+1] << 8 | engGE.space[engGE.pos]) & engGE.solid);

}//end of checkOverlap

/** deletes the full levels */
static void engKillFullLevels(void)
{
   // loop counter
   int t, tn;

   // for every level
   for(t = 0; t < SPACELENGTH; t++)
   {
      // if full level found
      if (engGE.space[t] == 0xff) 
      {
         // increase score
         engGE.score += engScoreStep[engGE.game_opts.diff];
                   
         // step down every higher level
         for (tn = t+1; tn <= SPACELENGTH; tn++)
         {
            // get the next level or 0 on the top level
            engGE.space[tn-1] = engGE.space[tn] * 
                         (tn < SPACELENGTH);

         } // end of every level

      } // end of if full

   } // end of every level

} //end of checkFullLevels

/** lower the solid with one level
   \return false if invalid (end of game) */
bool engLowerSolid(void)
{
  // set the new solid indicator false
  engGE.isnewsolid = 0;

  // solid reached the floor flag
  bool onFloor = (engGE.pos <= 0) ||
                 ( (engGE.pos-- || 1) &&
                   engCheckOverlap() &&
                   (engGE.pos++ || 1) );

  // if reached the floor,
  if (onFloor)
  {
    // put the solid to the space
    engGE.space[engGE.pos]   |= engGE.solid;
    engGE.space[engGE.pos+1] |= engGE.solid >> 8;
               
    // delete the full levels
    engKillFullLevels();
    // get new solid
    engNewSolid();

    // check new solid already overlapped
    engGE.gameOver = engCheckOverlap();
    return (engGE.gameOver);
  }

  return(0);
}

/** turns the solid from axis 1 to axis 2
   \return indicator of turn availability */
bool engTurn(char ax1, char ax2)
{
   // loop cnt, temp var
   int n, n2;

   // remove the solid to a temp solid
   TSolid tempSolid = engGE.solid;
   engGE.solid = 0x0000;

   // turn it

   // for every cell
   for (n = 0; n < 16; n++)
   {
     // the bits of the number of the element are 
     // the coords of the element - so the num = pos vector
     
     // turn the vector (replace the two bit specified by axis num,
     //    and negate the second)
     n2 = (((n & (~(0x01 << ax2)))
            & (~(0x01 << ax1)))
            | (   ((n >> ax1) & 0x01) << ax2))
            | ( (!((n >> ax2) & 0x01)) << ax1);

      // get the element to the new turned place
      engGE.solid |= ((tempSolid >> n) & 0x01) << n2;
   }

   // put to the lower level of the solid
   // if lower level empty then
   if (engGE.solid && 0x00ff == 0x00)
      // move from the higher level to the lower
      engGE.solid = engGE.solid >> 8;

   // if overlapped, invalid turn
   // get back the original
   return !(engCheckOverlap() && (engGE.solid = tempSolid)); 
 
}


