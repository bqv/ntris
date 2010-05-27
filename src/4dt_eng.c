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
#include <stdio.h>
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

/** Empty solid */
static const tEngSolid engEmptySolid =
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{0,0}, {0,1}}}};

/** Empty level */
static const tEngLevel engEmptyLevel = {{{0,0}, {0,0}}, {{0,0}, {0,0}}};

/** Full level */
static const tEngLevel engFullLevel = {{{1,1}, {1,1}}, {{1,1}, {1,1}}};

/** defined solids */
static const tEngSolid engSolids[SOLIDTYPES] =
{
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{0,0}, {0,1}}}},
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{0,0}, {1,1}}}},
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{0,1}, {1,1}}}},
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,0}}, {{1,1}, {1,1}}}},
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {1,0}}, {{0,1}, {1,1}}}},
  {{{{0,0}, {0,0}}, {{0,0}, {0,0}}}, {{{0,0}, {0,1}}, {{0,1}, {1,1}}}},
};

/** probabilities of solids in different
    difficulty levels (easy, medium, hard) */
static const int engProbs[DIFFLEVELS][SOLIDTYPES] =
{{10, 5, 5, 1, 1, 1},
 { 5, 3, 2, 1, 1, 1},
 { 2, 1, 1, 1, 1, 1}};

/** Score points for filling a level in different difficulty levels */
static const int engScoreStep[DIFFLEVELS] = { 100, 200, 400};

/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

/** Game engine container */
tEngGame engGE;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void engNewSolid(void);
static int engOverlapping(void);
static void engKillFullLevels(void);
static int engEqLevel(tEngLevel level1, tEngLevel level2);
static void engCopyLevel(tEngLevel target, tEngLevel source);
static void engCopySolid(tEngSolid target, tEngSolid source);

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

void engPrintSpace(void)
{
  int w, x, y, z;

  for(y = YSIZE-1; y >= 0; y--)
  {
    for(z = ZSIZE-1; z >= 0 ; z--)
    {
      printf((z == 1) ? " " : "");
      for(w = 0; w < SPACELENGTH; w++)
      {
        for(x = 0; x < XSIZE; x++)
        {
          printf(engGE.space[w][x][y][z] ||
                 (  (w - engGE.pos >= 0) && (w - engGE.pos < WSIZE)
                     && engGetSolidCell(w - engGE.pos, x, y, z))
                 ? "X" : ".");
          printf("  ");
        }
        printf(" ");
      }
      printf("\n");
    }
  }
  printf("\n");
}

/** Compare two levels */
static int engEqLevel(tEngLevel level1, tEngLevel level2)
{
  int x, y, z;
  int equal = 1;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    if (level1[x][y][z] != level2[x][y][z]) equal = 0;
  }

  return(equal);
}

/** Copy level to another. */
static void engCopyLevel(tEngLevel target, tEngLevel source)
{
  int x, y, z;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    target[x][y][z] = source[x][y][z];
  }
}

/** Clears a levels */
static void engClearLevel(tEngLevel level)
{
  int x, y, z;

  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    level[x][y][z] = 0;
  }
}

/** duplicates a solid */
static void engCopySolid(tEngSolid target, tEngSolid source)
{
  int w;

  for (w = 0; w < WSIZE; w++)
  {
    engCopyLevel(target[w], source[w]);
  }
}

/** duplicates the game engine */
tEngGame engCopyGameEngine(tEngGame inGameEngine)
{
  int w;
  tEngGame result; // duplicated game engine;

  // Copy the input game engine to the result one.
  result = inGameEngine;

  // Copy the gamespace to the result.
  for (w = 0; w < SPACELENGTH; w++)
  {
    engCopyLevel(result.space[w], inGameEngine.space[w]);
  }

  // return with the result gamespace
  return result;
}

/** Reset game variables */
void engResetGame(void)
{
  int w;

  // for the every part of the space
  for (w = 0; w < SPACELENGTH; w++)
  {
    engClearLevel(engGE.space[w]);
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

   engCopySolid(engGE.solid, engSolids[i]);

   // position the new solid to the
   // top 2 level of the space
   engGE.pos = SPACELENGTH - WSIZE;

   // increase the number of the solid
   engGE.solidnum++;
}

/** check overlap between solid and gamespace
 *  \return overlapping detected flag */
static int engOverlapping(void)
{
  int w, x, y, z;
  int overlap = 0;

  for(w = 0; w < WSIZE; w++)
  for(x = 0; x < XSIZE; x++)
  for(y = 0; y < YSIZE; y++)
  for(z = 0; z < ZSIZE; z++)
  {
    overlap |= engGE.space[engGE.pos+w][x][y][z] && !!engGetSolidCell(w, x, y, z);
  }

  return(overlap);

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
      if (engEqLevel(engGE.space[t], engFullLevel))
      {
         // increase score
         engGE.score += engScoreStep[engGE.game_opts.diff];

         // step down every higher level
         for (tn = t+1; tn < SPACELENGTH; tn++)
         {
            // get the next level
            engCopyLevel(engGE.space[tn-1], engGE.space[tn]);
         } // end of every level
         // 0 on the top level
         engClearLevel(engGE.space[SPACELENGTH-1]);

      } // end of if full

   } // end of every level

} //end of checkFullLevels

/** lower the solid with one level
   \return false if invalid (end of game) */
int engLowerSolid(void)
{
  int w, x, y, z;

  // solid reached the floor flag
  int onFloor = (engGE.pos <= 0) ||
                ( (engGE.pos-- || 1) &&
                  engOverlapping() &&
                  (engGE.pos++ || 1) );

  // if reached the floor,
  if (onFloor)
  {
    // put the solid to the space
    for(w = 0; w < WSIZE; w++)
    for(x = 0; x < XSIZE; x++)
    for(y = 0; y < YSIZE; y++)
    for(z = 0; z < ZSIZE; z++)
    {
      engGE.space[engGE.pos+w][x][y][z] |= engGetSolidCell(w, x, y, z);
    }

    // delete the full levels
    engKillFullLevels();
    // get new solid
    engNewSolid();

    // check new solid already overlapped
    engGE.gameOver = engOverlapping();
    return (engGE.gameOver);
  }

  return(0);
}

/** turns the solid from axis 1 to axis 2
   \return indicator of turn availability */
int engTurn(char ax1, char ax2)
{
   // loop cnt, temp var
   int r1[4];
   int r2[4];
   int tmp;

   // remove the solid to a temp solid
   tEngSolid tempSolid;
   engCopySolid(tempSolid, engGE.solid);
   engCopySolid(engGE.solid, engEmptySolid);

   // turn it

   // for every cell
   for(r1[0] = 0; r1[0] < WSIZE; r1[0]++)
   for(r1[1] = 0; r1[1] < XSIZE; r1[1]++)
   for(r1[2] = 0; r1[2] < YSIZE; r1[2]++)
   for(r1[3] = 0; r1[3] < ZSIZE; r1[3]++)
   {
     // the bits of the number of the element are
     // the coords of the element - so the num = pos vector

     // turn the vector (replace the two bit specified by axis num,
     // and negate the second)
     r2[0] = r1[0];
     r2[1] = r1[1];
     r2[2] = r1[2];
     r2[3] = r1[3];

     tmp   = r2[ax1];
     r2[ax1] = !r2[ax2];
     r2[ax2] = tmp;

      // get the element to the new turned place
      engGE.solid[r2[0]][r2[1]][r2[2]][r2[3]] =
          tempSolid[r1[0]][r1[1]][r1[2]][r1[3]];
   }

   // put to the lower level of the solid
   // if lower level empty then
   if (engEqLevel(engGE.solid[0], engEmptyLevel))
   {
     // move from the higher level to the lower
     engCopyLevel(engGE.solid[0], engGE.solid[1]);
     engCopyLevel(engGE.solid[1], engEmptyLevel);
   }

   // if overlapped, invalid turn
   // get back the original
   if (engOverlapping())
   {
     engCopySolid(engGE.solid, tempSolid);
     return(0);
   }
   else
   {
     return(1);
   }
}
